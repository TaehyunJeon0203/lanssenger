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
            io_context_.run();
        });
        // 서버로부터 메시지를 받는 스레드 시작
        receive_thread_ = std::thread([this]() {
            try {
                while (running_) {
                    boost::asio::streambuf buf;
                    boost::asio::read_until(socket_, buf, '\n');
                    std::string message;
                    std::istream is(&buf);
                    std::getline(is, message);
                    
                    if (message_callback_) {
                        message_callback_(message);
                    } else {
                        std::cout << message << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                if (running_) {
                    std::cerr << "수신 오류: " << e.what() << std::endl;
                }
            }
        });

        // 콘솔 모드일 때만 사용자 입력을 처리
        if (!message_callback_) {
            std::string input;
            while (running_) {
                std::getline(std::cin, input);
                if (input == "/quit") {
                    break;
                }
                sendMessage(input);
            }
            stop();
        } else {
            // GUI 모드일 때는 종료 방지: 메인 스레드 종료 안 되게 유지
            // 메시지 콜백이 있는 경우 GUI 루프가 메인 루프를 유지해주므로
            // 여기서는 receive_thread_만 백그라운드로 유지되면 OK
            // 추가적인 sleep 루프를 통해 메인 스레드가 빠르게 종료되지 않게 함
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
            std::cout << "[클라이언트] sendMessage 호출됨: " << message << std::endl;  // 로그 출력
            boost::asio::write(socket_, boost::asio::buffer(message + "\n"));
        } catch (const std::exception& e) {
            std::cerr << "전송 오류: " << e.what() << std::endl;
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
