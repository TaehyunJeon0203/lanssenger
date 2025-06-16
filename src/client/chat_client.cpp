#include "client/chat_client.hpp"
#include <iostream>

ChatClient::ChatClient() : socket_(io_context_), running_(false) {}

ChatClient::~ChatClient() {
    stop();
}

bool ChatClient::connect(const std::string& host, unsigned short port) {
    try {
        // DNS 리졸버를 생성하여 호스트 이름을 IP 주소로 변환
        boost::asio::ip::tcp::resolver resolver(io_context_);
        // 호스트 이름과 포트를 사용하여 연결 가능한 엔드포인트 목록을 가져옴
        auto endpoints = resolver.resolve(host, std::to_string(port));
        
        // 가져온 엔드포인트 중 하나에 연결 시도
        boost::asio::connect(socket_, endpoints);
        std::cout << "서버에 연결되었습니다." << std::endl;
        
        // 연결 성공 시 클라이언트 실행 상태를 활성화
        running_ = true;
        return true;
    } catch (const std::exception& e) {
        // 연결 실패 시 오류 메시지 출력
        std::cerr << "연결 실패: " << e.what() << std::endl;
        return false;
    }
}

void ChatClient::setMessageCallback(MessageCallback callback) {
    message_callback_ = std::move(callback);
}

void ChatClient::start() {
    // io_context 실행 스레드 추가
    io_thread_ = std::thread([this]() {
        try {
            io_context_.run();
        } catch (const std::exception& e) {
            std::cerr << "IO Context 오류: " << e.what() << std::endl;
        }
    });

    // 서버로부터 메시지를 받는 스레드 시작
    receive_thread_ = std::thread([this]() {
        try {
            while (running_) {
                boost::asio::streambuf buf;
                boost::system::error_code ec;
                
                // 비동기 읽기 대신 동기 읽기 사용
                size_t len = boost::asio::read_until(socket_, buf, '\n', ec);
                
                if (ec) {
                    if (ec == boost::asio::error::eof) {
                        std::cout << "서버와의 연결이 종료되었습니다." << std::endl;
                    } else {
                        std::cerr << "수신 오류: " << ec.message() << std::endl;
                    }
                    if (running_) {
                        // 재연결 시도
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        continue;
                    }
                    break;
                }

                if (len > 0) {
                    std::string message;
                    std::istream is(&buf);
                    std::getline(is, message);
                    
                    if (!message.empty()) {
                        if (message_callback_) {
                            try {
                                message_callback_(message);
                            } catch (const std::exception& e) {
                                std::cerr << "메시지 콜백 처리 오류: " << e.what() << std::endl;
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            if (running_) {
                std::cerr << "수신 스레드 오류: " << e.what() << std::endl;
            }
        }
    });
}

void ChatClient::stop() {
    running_ = false;
    io_context_.stop();
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
    if (io_thread_.joinable()) {
        io_thread_.join();
    }
    if (socket_.is_open()) {
        socket_.close();
    }
}

void ChatClient::sendMessage(const std::string& message) {
    try {
        if (!socket_.is_open()) {
            std::cerr << "소켓이 닫혀있습니다." << std::endl;
            return;
        }
        
        boost::system::error_code ec;
        boost::asio::write(socket_, boost::asio::buffer(message + "\n"), ec);
        
        if (ec) {
            std::cerr << "전송 오류: " << ec.message() << std::endl;
            stop();
        }
    } catch (const std::exception& e) {
        std::cerr << "전송 예외 발생: " << e.what() << std::endl;
        stop();
    }
} 