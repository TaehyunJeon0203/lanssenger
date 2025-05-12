#pragma once

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <string>
#include <memory>
#include <thread>
#include <mutex>

// LAN 메신저의 클라이언트 클래스
// 서버와의 통신을 담당하며, 메시지 전송 및 수신 기능을 제공
class Client {
public:
    // 생성자
    Client();
    // 소멸자
    ~Client();

    // 서버에 연결
    bool connectToServer(const std::string& host, unsigned short port);
    // 서버와 연결 해제
    void disconnectFromServer();
    // 메시지 전송
    bool sendMessage(const std::string& message);
    // 연결 상태 확인
    bool isConnected() const;

    // 시그널 정의
    boost::signals2::signal<void(const std::string&)> messageReceived;
    boost::signals2::signal<void()> connected;
    boost::signals2::signal<void()> disconnected;
    boost::signals2::signal<void(const std::string&)> error;

private:
    // 데이터 읽기
    void readData();
    // 에러 처리
    void handleError(const boost::system::error_code& error);

    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket;
    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
    std::thread io_thread;
    std::mutex mutex;
    bool connected_;
    static const size_t MAX_BUFFER_SIZE = 4096;
}; 