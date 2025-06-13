#include "server/server.hpp"
#include "common/chat_room.hpp"
#include <iostream>

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

namespace lanssenger {

Server::Server()
    : running_(false)
{
}

Server::~Server()
{
    stop();
}

bool Server::start(unsigned short port)
{
    if (running_) {
        // Server is already running
        return false;
    }

    boost::system::error_code ec;
    try {
        // 1. Acceptor 초기화 및 바인딩
        acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
        
        // 2. 주소 재사용 옵션 설정
        acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        
        // 3. 연결 수락 대기 시작
        acceptor->listen();
        
        // 4. 비동기 연결 수락 시작
        handleNewConnection();
        
        // 5. IO 컨텍스트 실행 스레드 시작
        io_thread = std::thread([this]() { io_context.run(); });
        
        running_ = true;
        std::cout << "Server started on port " << port << std::endl;
        return true;

    } catch (const boost::system::system_error& e) {
        std::cerr << "Server start error: " << e.what() << std::endl;
        running_ = false;
        return false;
    }
}

void Server::stop()
{
    if (!running_) {
        return;
    }

    // 1. Acceptor 닫기 (더 이상 새로운 연결을 받지 않음)
    if (acceptor) {
        boost::system::error_code ec;
        acceptor->close(ec);
        // 오류 처리는 필요에 따라 추가
    }

    // 2. 모든 클라이언트 소켓 닫기
    for (auto const& [clientId, socket] : clients_) {
        if (socket->is_open()) {
            boost::system::error_code ec;
            socket->close(ec);
        }
    }
    clients_.clear();

    // 3. io_context 중지 (진행 중인 모든 비동기 작업 취소)
    io_context.stop();

    // 4. IO 스레드 종료 대기
    if (io_thread.joinable()) {
        io_thread.join();
    }

    running_ = false;
    std::cout << "Server stopped." << std::endl;
}

bool Server::isRunning() const
{
    return running_;
}

void Server::handleNewConnection()
{
    // 1. 새로운 소켓 생성 (새로운 연결을 받을 준비)
    auto new_socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    
    // 2. 비동기 accept 작업 시작
    acceptor->async_accept(*new_socket, 
        [this, new_socket](const boost::system::error_code& ec)
    {
        if (!ec) {
            // 3. 연결 성공 시 처리
            std::string clientId = new_socket->remote_endpoint().address().to_string() + ":" 
                                + std::to_string(new_socket->remote_endpoint().port());
            
            std::cout << "New connection from " << clientId << std::endl;
            
            // ActiveUsersManager에 사용자 추가
            std::string ipAddress = new_socket->remote_endpoint().address().to_string();
            ActiveUsersManager::getInstance().addUser(clientId, "User" + std::to_string(clients_.size() + 1), ipAddress);
            
            // 클라이언트 맵에 추가
            clients_[clientId] = new_socket;
            std::cout << "Total connected clients: " << clients_.size() << std::endl;
            
            // 접속자 목록 브로드캐스트
            broadcastActiveUsers();
            
            // 클라이언트로부터 데이터 읽기 시작
            startReading(new_socket, clientId);
            
            // 4. 다음 연결을 위해 다시 accept 대기
            handleNewConnection();
        } else {
            // 5. 오류 발생 시 처리
            std::cerr << "Accept error: " << ec.message() << std::endl;
            if (ec == boost::asio::error::operation_aborted) {
                // 서버가 중지 중인 경우
                return;
            }
            // 다른 오류의 경우 다음 연결 시도
            handleNewConnection();
        }
    });
}

void Server::startReading(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const std::string& clientId)
{
    // 클라이언트로부터 데이터를 읽기 위한 버퍼
    auto buffer = std::make_shared<boost::asio::streambuf>();
    
    // 비동기 읽기 시작
    boost::asio::async_read_until(*socket, *buffer, '\n',
        [this, socket, clientId, buffer](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        if (!ec) {
            // 데이터를 성공적으로 읽은 경우
            std::string data(boost::asio::buffers_begin(buffer->data()),
                           boost::asio::buffers_begin(buffer->data()) + bytes_transferred);
            
            // 버퍼에서 읽은 데이터 제거
            buffer->consume(bytes_transferred);
            
            // 데이터 처리
            handleClientData(clientId, data);
            
            // 다음 데이터 읽기 시작
            startReading(socket, clientId);
        } else {
            // 오류 발생 시 (연결 종료 등)
            std::cerr << "Read error from client " << clientId << ": " << ec.message() << std::endl;
            handleClientDisconnection(clientId);
        }
    });
}

void Server::handleClientDisconnection(const std::string& clientId)
{
    // 클라이언트 맵에서 제거
    if (clients_.erase(clientId) > 0) {
        std::cout << "Client " << clientId << " disconnected" << std::endl;
        std::cout << "Remaining clients: " << clients_.size() << std::endl;
        
        // ActiveUsersManager에서 사용자 제거
        ActiveUsersManager::getInstance().removeUser(clientId);
        
        // 접속자 목록 브로드캐스트
        broadcastActiveUsers();
    }
}

void Server::handleClientData(const std::string& clientId, const std::string& data)
{
    std::string trimmedData = data;
    if (!trimmedData.empty() && trimmedData.back() == '\n') {
        trimmedData.pop_back();
    }

    if (trimmedData.empty()) {
        return;
    }

    if (trimmedData == "/users") {
        broadcastActiveUsers();
    }
    else if (trimmedData.find("/nickname ") == 0) {
        std::string nickname = trimmedData.substr(10);
        std::cout << "[서버] 닉네임 설정 요청: " << nickname << std::endl;

        auto& manager = ActiveUsersManager::getInstance();
        manager.updateNickname(clientId, nickname);
        
        // 닉네임 변경 후 유저 목록 브로드캐스트
        broadcastActiveUsers();
    }
    else if (trimmedData.find("/create_room ") == 0) {
        std::istringstream iss(trimmedData);
        std::string cmd, roomName, opt;
        bool isPrivate = false;
        std::string password = "";

        iss >> cmd >> roomName;

        while (iss >> opt) {
            if (opt == "--private") isPrivate = true;
            else if (opt == "--password") iss >> password;
        }

        bool created = ChatRoomManager::getInstance().createRoom(roomName, isPrivate, password);

        std::string response = created
            ? "채팅방 [" + roomName + "] 생성 완료\n"
            : "채팅방 생성 실패: 같은 이름의 방이 존재합니다\n";

        auto socket = clients_[clientId];
        if (socket && socket->is_open()) {
            boost::asio::write(*socket, boost::asio::buffer(response + "\n"));
        }
    }
    else if (trimmedData.find("/list_rooms") == 0) {
        std::cout << "[서버] /list_rooms 요청 처리 중!" << std::endl;

        std::vector<std::string> publicRooms = ChatRoomManager::getInstance().getPublicRooms();
        std::vector<std::string> privateRooms = ChatRoomManager::getInstance().getPrivateRooms();

        std::string response = "ROOM_LIST:";
        
        // 공개 방 목록 추가
        for (const auto& room : publicRooms) {
            response += "PUBLIC:" + room + ",";
        }
        
        // 비공개 방 목록 추가
        for (const auto& room : privateRooms) {
            response += "PRIVATE:" + room + ",";
        }
        
        if (!publicRooms.empty() || !privateRooms.empty()) {
            response.pop_back(); // 마지막 쉼표 제거
        }
        response += "\n";

        auto socket = clients_[clientId];
        if (socket && socket->is_open()) {
            boost::system::error_code ec;
            boost::asio::write(*socket, boost::asio::buffer(response), ec);
            if (ec) {
                std::cerr << "[서버] 방 목록 전송 실패: " << ec.message() << std::endl;
                handleClientDisconnection(clientId);
            } else {
                std::cout << "[서버] 방 목록 전송 완료" << std::endl;
            }
        }
    }
    else if (trimmedData.find("/room_msg ") == 0) {
        std::istringstream iss(trimmedData);
        std::string cmd, roomName, message;
        iss >> cmd >> roomName;
        
        // 나머지 메시지 부분을 모두 읽음
        std::getline(iss, message);
        message = message.substr(1); // 앞의 공백 제거
        
        // 현재 사용자가 참여 중인 방인지 확인
        auto* roomInfo = ChatRoomManager::getInstance().getRoomInfo(roomName);
        if (roomInfo && roomInfo->members.find(clientId) != roomInfo->members.end()) {
            // 방의 모든 멤버에게 메시지 전송
            std::string nickname = ActiveUsersManager::getInstance().getNickname(clientId);
            std::string ipLastThree = ActiveUsersManager::getInstance().getAllActiveUsers()[clientId].ipLastThree;
            std::string formattedMessage = "ROOM_MSG:" + nickname + "(" + ipLastThree + "): " + message + "\n";
            
            for (const auto& memberId : roomInfo->members) {
                auto memberSocket = clients_[memberId];
                if (memberSocket && memberSocket->is_open()) {
                    boost::asio::write(*memberSocket, boost::asio::buffer(formattedMessage));
                }
            }
        }
    }
    else if (trimmedData.find("/room_users ") == 0) {
        std::string roomName = trimmedData.substr(11);
        roomName = trim(roomName);
        std::cout << "[서버] 방 사용자 목록 요청: [" << roomName << "]" << std::endl;
        
        auto* roomInfo = ChatRoomManager::getInstance().getRoomInfo(roomName);
        if (roomInfo) {
            std::string response = "ROOM_USER_LIST:";
            bool hasUsers = false;
            for (const auto& userId : roomInfo->members) {
                const auto& activeUsers = ActiveUsersManager::getInstance().getAllActiveUsers();
                auto it = activeUsers.find(userId);
                if (it != activeUsers.end()) {
                    response += it->second.nickname + "(" + it->second.ipLastThree + "),";
                    hasUsers = true;
                }
            }
            if (hasUsers) {
                response.pop_back(); // 마지막 쉼표 제거
            }
            response += "\n";

            std::cout << "[서버] 방 사용자 목록 응답: [" << response << "]" << std::endl;

            auto socket = clients_[clientId];
            if (socket && socket->is_open()) {
                boost::asio::write(*socket, boost::asio::buffer(response));
            }
        } else {
            std::cout << "[서버] 방을 찾을 수 없음: [" << roomName << "]" << std::endl;
        }
    }
    else if (trimmedData.find("/join_room ") == 0) {
        std::string raw = trimmedData.substr(11);  // "/join_room " 이후 문자열
        std::string roomName;
        std::string password;

        size_t pwPos = raw.find("--password");
        if (pwPos != std::string::npos) {
            roomName = raw.substr(0, pwPos);
            password = raw.substr(pwPos + 10);  // "--password"는 10글자
            password = trim(password);
        } else {
            roomName = raw;
            password = "";  // 공백이거나 공개방
        }

        roomName = trim(roomName);
        password = trim(password);

        std::cout << "[서버] 방 참여 요청: [" << roomName << "] (password: [" << password << "])\n";

        bool joined = ChatRoomManager::getInstance().joinRoom(roomName, clientId, password);

        std::string response = joined
            ? "채팅방 [" + roomName + "] 참여 완료\n"
            : "채팅방 참여 실패: 비밀번호가 틀렸거나 방이 존재하지 않습니다\n";

        auto socket = clients_[clientId];
        if (socket && socket->is_open()) {
            boost::asio::write(*socket, boost::asio::buffer(response));
        }
    }
    else if (trimmedData.find("/leave_room ") == 0) {
        std::string roomName = trimmedData.substr(11);  // "/leave_room " 이후 문자열
        roomName = trim(roomName);

        std::cout << "[서버] 방 퇴장 요청: [" << roomName << "] from [" << clientId << "]\n";

        bool left = ChatRoomManager::getInstance().leaveRoom(roomName, clientId);

        std::string response = left
            ? "채팅방 [" + roomName + "] 퇴장 완료\n"
            : "채팅방 퇴장 실패: 방이 존재하지 않거나 이미 퇴장한 상태입니다\n";

        auto socket = clients_[clientId];
        if (socket && socket->is_open()) {
            boost::asio::write(*socket, boost::asio::buffer(response));
        }
    }
    else {
        // 일반 메시지 처리
        auto& userManager = ActiveUsersManager::getInstance();
        std::string nickname = userManager.getNickname(clientId);
        std::string ipLastThree = userManager.isUserActive(clientId)
            ? userManager.getAllActiveUsers()[clientId].ipLastThree
            : clientId.substr(clientId.find_last_of(":") + 1);

        std::string message = nickname + "(" + ipLastThree + "): " + trimmedData + "\n";
        for (const auto& [id, socket] : clients_) {
            if (socket && socket->is_open()) {
                try {
                    boost::system::error_code ec;
                    boost::asio::write(*socket, boost::asio::buffer(message), ec);
                    
                    if (ec) {
                        std::cerr << "[서버] 메시지 전송 실패: " << ec.message() << std::endl;
                        handleClientDisconnection(id);
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[서버] 메시지 전송 중 오류: " << e.what() << std::endl;
                    handleClientDisconnection(id);
                }
            }
        }
    }
}

void Server::broadcastActiveUsers()
{
    auto activeUsers = ActiveUsersManager::getInstance().getAllActiveUsers();
    std::string userList = "USER_LIST:";
    
    for (const auto& [userId, info] : activeUsers) {
        userList += info.nickname + "(" + info.ipLastThree + "),";
    }
    
    if (!userList.empty() && userList.back() == ',') {
        userList.pop_back();
    }
    
    userList += "\n";
    
    std::cout << "[서버] 브로드캐스트 유저 목록: [" << userList << "]" << std::endl;
    
    for (const auto& [id, socket] : clients_) {
        if (socket && socket->is_open()) {
            try {
                boost::system::error_code ec;
                size_t written = boost::asio::write(*socket, boost::asio::buffer(userList), ec);
                
                if (ec) {
                    std::cerr << "[서버] 브로드캐스트 실패: " << ec.message() << std::endl;
                    handleClientDisconnection(id);
                } else {
                    std::cout << "[서버] 브로드캐스트 완료: " << written << " bytes" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "[서버] 브로드캐스트 중 오류: " << e.what() << std::endl;
                handleClientDisconnection(id);
            }
        }
    }
}

} // namespace lanssenger 