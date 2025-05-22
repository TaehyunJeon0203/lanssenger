#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <memory>
#include <unordered_map>

class Server {
public:
    Server();
    ~Server();

    // 서버 시작/중지
    bool start(unsigned short port);
    void stop();
    bool isRunning() const;

private:
    // 서버 상태
    bool running_;
    
    // Boost ASIO 관련 멤버
    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::thread io_thread;
    
    // 클라이언트 관리
    std::unordered_map<std::string, std::shared_ptr<boost::asio::ip::tcp::socket>> clients;
    
    // 클라이언트 연결 처리
    void handleNewConnection();
    void handleClientDisconnection(const std::string& clientId);
    void handleClientData(const std::string& clientId, const std::string& data);
}; 