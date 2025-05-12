#pragma once

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <string>
#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>

// LAN 메신저의 서버 클래스
// 클라이언트의 연결을 관리하고 메시지를 브로드캐스팅하는 기능을 제공
class Server {
public:
    // 생성자
    Server();
    // 소멸자
    ~Server();

    // 서버 시작
    bool start(unsigned short port);
    // 서버 중지
    void stop();
    // 서버 실행 상태 확인
    bool isRunning() const;

    // 시그널 정의
    boost::signals2::signal<void(const std::string&)> clientConnected;
    boost::signals2::signal<void(const std::string&)> clientDisconnected;
    boost::signals2::signal<void(const std::string&, const std::string&)> messageReceived;
    boost::signals2::signal<void(const std::string&)> error;

private:
    // 새로운 연결 처리
    void handleNewConnection();
    // 클라이언트 연결 해제 처리
    void handleClientDisconnection(const std::string& clientId);
    // 클라이언트 데이터 처리
    void handleClientData(const std::string& clientId, const std::string& data);

    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::unordered_map<std::string, std::unique_ptr<boost::asio::ip::tcp::socket>> clients;
    std::thread io_thread;
    std::mutex mutex;
    bool running_;
    static const size_t MAX_BUFFER_SIZE = 4096;
}; 