#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <mutex>
#include "../include/common.hpp"

using boost::asio::ip::tcp;

class Client {
private:
    boost::asio::io_context io_context;
    tcp::socket socket;
    std::string username;
    std::mutex cout_mutex;
    bool running;

    void start_receive() {
        auto message = std::make_shared<Message>();
        boost::asio::async_read(socket,
            boost::asio::buffer(reinterpret_cast<char*>(message.get()), sizeof(Message)),
            [this, message](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error && bytes_transferred > 0) {
                    handle_message(message);
                    start_receive();
                } else {
                    std::lock_guard<std::mutex> lock(cout_mutex);
                    std::cout << "서버와의 연결이 끊어졌습니다." << std::endl;
                    running = false;
                }
            });
    }

    void handle_message(std::shared_ptr<Message> message) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        switch (message->type) {
            case MessageType::CHAT:
                std::cout << message->sender << ": " << message->content << std::endl;
                break;
            case MessageType::SYSTEM:
                std::cout << "[시스템] " << message->content << std::endl;
                break;
            default:
                break;
        }
    }

public:
    Client() : socket(io_context), running(false) {}

    bool connect(const std::string& host, const std::string& name) {
        try {
            username = name;
            tcp::resolver resolver(io_context);
            auto endpoints = resolver.resolve(host, std::to_string(SERVER_PORT));
            boost::asio::connect(socket, endpoints);

            // 연결 메시지 전송
            Message connect_msg;
            connect_msg.type = MessageType::CONNECT;
            strncpy(connect_msg.sender, username.c_str(), sizeof(connect_msg.sender) - 1);
            strncpy(connect_msg.content, "연결됨", sizeof(connect_msg.content) - 1);
            
            boost::asio::write(socket, boost::asio::buffer(&connect_msg, sizeof(Message)));
            
            running = true;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "연결 실패: " << e.what() << std::endl;
            return false;
        }
    }

    void run() {
        // 메시지 수신 스레드 시작
        std::thread receive_thread([this]() {
            start_receive();
            io_context.run();
        });

        // 메시지 입력 및 전송
        std::string input;
        while (running && std::getline(std::cin, input)) {
            if (input == "quit") {
                break;
            }

            Message msg;
            msg.type = MessageType::CHAT;
            strncpy(msg.sender, username.c_str(), sizeof(msg.sender) - 1);
            strncpy(msg.content, input.c_str(), sizeof(msg.content) - 1);

            try {
                boost::asio::write(socket, boost::asio::buffer(&msg, sizeof(Message)));
            } catch (const std::exception& e) {
                std::cerr << "메시지 전송 실패: " << e.what() << std::endl;
                break;
            }
        }

        // 정상 종료
        running = false;
        io_context.stop();
        receive_thread.join();
    }
};

int main() {
    std::string username;
    std::cout << "사용자 이름을 입력하세요: ";
    std::getline(std::cin, username);

    Client client;
    if (client.connect("127.0.0.1", username)) {
        std::cout << "서버에 연결되었습니다." << std::endl;
        client.run();
    }

    return 0;
} 