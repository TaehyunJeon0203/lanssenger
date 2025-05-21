#include "server/server.hpp"

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

    // TODO: Implement server stop logic (close acceptor, sockets, stop io_context, join thread)

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
            // 3. 연결 성공 시 처리 (클라이언트 관리 맵에 추가 등)
            std::cout << "New connection from " << new_socket->remote_endpoint() << std::endl;
            // TODO: Add client to map and start reading data
            
            // 4. 다음 연결을 위해 다시 accept 대기
            handleNewConnection();
        } else {
            // 5. 오류 발생 시 처리
            std::cerr << "Accept error: " << ec.message() << std::endl;
            // TODO: Handle error (maybe stop server if critical)
        }
    });
}

void Server::handleClientDisconnection(const std::string& clientId)
{
    // TODO: Implement client disconnection handling
}

void Server::handleClientData(const std::string& clientId, const std::string& data)
{
    // TODO: Implement client data handling
} 