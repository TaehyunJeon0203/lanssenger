#include <iostream>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include <atomic>
#include <functional>

class ChatClient {
public:
    using MessageCallback = std::function<void(const std::string&)>;

    ChatClient() : socket_(io_context_), running_(false) {}

    bool connect(const std::string& host, unsigned short port) {
        try {
            boost::asio::ip::tcp::resolver resolver(io_context_);
            auto endpoints = resolver.resolve(host, std::to_string(port));
            
            boost::asio::connect(socket_, endpoints);
            std::cout << "서버에 연결되었습니다." << std::endl;
            
            running_ = true;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "연결 실패: " << e.what() << std::endl;
            return false;
        }
    }

    void setMessageCallback(MessageCallback callback) {
        message_callback_ = callback;
    }

    void start() {
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
                            std::cout << "[클라이언트] 받은 메시지: [" << message << "]" << std::endl;
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

        // GUI 모드일 때는 종료 방지
        if (message_callback_) {
            std::thread([this]() {
                while (running_) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }).detach();
        }
    }

    void stop() {
        running_ = false;
        io_context_.stop(); // io_context 중지 요청
        if (receive_thread_.joinable()) {
            receive_thread_.join();
        }
        if (socket_.is_open()) {
            socket_.close();
        }
    }

    void sendMessage(const std::string& message) {
        try {
            if (!socket_.is_open()) {
                std::cerr << "소켓이 닫혀있습니다." << std::endl;
                return;
            }

            std::cout << "[클라이언트] sendMessage 호출됨: [" << message << "]" << std::endl;
            
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

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
    std::thread receive_thread_;
    std::thread io_thread_; 
    std::atomic<bool> running_;
    MessageCallback message_callback_;
};

// 콘솔 클라이언트용 main 함수
#ifdef CONSOLE_CLIENT
int main() {
    try {
        ChatClient client;
        
        // 서버 연결
        if (!client.connect("localhost", 8080)) {
            return 1;
        }

        std::cout << "채팅을 시작합니다. 종료하려면 /quit를 입력하세요." << std::endl;
        client.start();

    } catch (const std::exception& e) {
        std::cerr << "오류 발생: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
#endif
