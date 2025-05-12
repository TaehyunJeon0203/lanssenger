#include "server/server.hpp"
#include <iostream>

// 서버 객체 생성자
// TCP 서버를 초기화하고 새로운 연결을 처리하기 위한 시그널/슬롯 연결을 설정
Server::Server() : running_(false) {
    acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(io_context);
}

// 서버 객체 소멸자
// 서버를 중지하고 모든 클라이언트 연결을 정리
Server::~Server() {
    stop();
}

// 서버를 시작
// 지정된 포트에서 클라이언트의 연결을 수락하기 시작
bool Server::start(unsigned short port) {
    try {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        acceptor->open(endpoint.protocol());
        acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor->bind(endpoint);
        acceptor->listen();
        
        running_ = true;
        
        // IO 스레드 시작
        io_thread = std::thread([this]() {
            try {
                io_context.run();
            } catch (const std::exception& e) {
                error(e.what());
            }
        });
        
        // 새로운 연결 수락 시작
        handleNewConnection();
        
        return true;
    } catch (const std::exception& e) {
        error(e.what());
        return false;
    }
}

// 서버를 중지
// 서버를 중지하고 모든 클라이언트 연결을 종료
void Server::stop() {
    if (running_) {
        try {
            acceptor->close();
            for (auto& client : clients) {
                client.second->close();
            }
            clients.clear();
        } catch (...) {}
        
        if (io_thread.joinable()) {
            io_context.stop();
            io_thread.join();
        }
        
        running_ = false;
    }
}

// 서버의 실행 상태를 확인
bool Server::isRunning() const {
    return running_;
}

// 새로운 클라이언트 연결을 처리하는 슬롯
// 새로운 클라이언트가 연결되면 필요한 시그널/슬롯 연결을 설정하고 클라이언트 목록에 추가
void Server::handleNewConnection() {
    auto socket = std::make_unique<boost::asio::ip::tcp::socket>(io_context);
    auto socket_ptr = socket.get();
    
    acceptor->async_accept(*socket,
        [this, socket = std::move(socket)](const boost::system::error_code& error) {
            if (!error) {
                std::string clientId = socket->remote_endpoint().address().to_string() + ":" +
                                     std::to_string(socket->remote_endpoint().port());
                
                clients[clientId] = std::move(socket);
                clientConnected(clientId);
                
                // 클라이언트로부터 데이터 읽기 시작
                handleClientData(clientId, "");
            } else {
                this->error(error.message());
            }
            
            // 다음 연결 수락
            if (running_) {
                handleNewConnection();
            }
        }
    );
}

// 클라이언트 연결 해제를 처리하는 슬롯
// 클라이언트가 연결을 해제하면 클라이언트 목록에서 제거하고 관련 리소스를 정리
void Server::handleClientDisconnection(const std::string& clientId) {
    auto it = clients.find(clientId);
    if (it != clients.end()) {
        try {
            it->second->close();
        } catch (...) {}
        clients.erase(it);
        clientDisconnected(clientId);
    }
}

// 클라이언트로부터의 데이터를 처리하는 슬롯
// 클라이언트로부터 메시지를 수신하면 다른 모든 클라이언트에게 브로드캐스팅
void Server::handleClientData(const std::string& clientId, const std::string& data) {
    auto it = clients.find(clientId);
    if (it == clients.end()) return;
    
    auto buffer = std::make_shared<std::array<char, MAX_BUFFER_SIZE>>();
    
    it->second->async_read_some(
        boost::asio::buffer(*buffer),
        [this, clientId, buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::string message(buffer->data(), bytes_transferred);
                messageReceived(clientId, message);
                
                // 모든 클라이언트에게 메시지 브로드캐스팅
                for (const auto& client : clients) {
                    if (client.first != clientId) {
                        try {
                            boost::asio::write(*client.second, 
                                boost::asio::buffer(clientId + ": " + message));
                        } catch (...) {}
                    }
                }
                
                // 다음 데이터 읽기
                handleClientData(clientId, "");
            } else if (error == boost::asio::error::eof) {
                handleClientDisconnection(clientId);
            } else {
                this->error(error.message());
                handleClientDisconnection(clientId);
            }
        }
    );
} 