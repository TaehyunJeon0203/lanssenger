#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include "../include/common.hpp"

using boost::asio::ip::tcp;

class Server {
private:
    boost::asio::io_context io_context;
    tcp::acceptor acceptor;
    std::vector<std::shared_ptr<tcp::socket>> clients;
    std::mutex clients_mutex;

    void start_accept() {
        auto socket = std::make_shared<tcp::socket>(io_context);
        acceptor.async_accept(*socket,
            [this, socket](const boost::system::error_code& error) {
                if (!error) {
                    handle_new_connection(socket);
                }
                start_accept();
            });
    }

    void handle_new_connection(std::shared_ptr<tcp::socket> socket) {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(socket);
        std::cout << "새로운 클라이언트 연결됨. 현재 연결 수: " << clients.size() << std::endl;
        
        // 클라이언트로부터 메시지 수신 시작
        start_receive(socket);
    }

    void start_receive(std::shared_ptr<tcp::socket> socket) {
        auto message = std::make_shared<Message>();
        boost::asio::async_read(*socket,
            boost::asio::buffer(reinterpret_cast<char*>(message.get()), sizeof(Message)),
            [this, socket, message](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error && bytes_transferred > 0) {
                    handle_message(socket, message);
                    start_receive(socket);
                } else {
                    remove_client(socket);
                }
            });
    }

    void handle_message(std::shared_ptr<tcp::socket> sender, std::shared_ptr<Message> message) {
        // 메시지를 모든 클라이언트에게 브로드캐스트
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto& client : clients) {
            if (client != sender) {
                boost::asio::async_write(*client,
                    boost::asio::buffer(reinterpret_cast<char*>(message.get()), sizeof(Message)),
                    [](const boost::system::error_code& error, std::size_t) {});
            }
        }
    }

    void remove_client(std::shared_ptr<tcp::socket> socket) {
        std::lock_guard<std::mutex> lock(clients_mutex);
        auto it = std::find(clients.begin(), clients.end(), socket);
        if (it != clients.end()) {
            clients.erase(it);
            std::cout << "클라이언트 연결 해제. 현재 연결 수: " << clients.size() << std::endl;
        }
    }

public:
    Server() : acceptor(io_context, tcp::endpoint(tcp::v4(), SERVER_PORT)) {
        std::cout << "서버 시작됨. 포트: " << SERVER_PORT << std::endl;
    }

    void run() {
        start_accept();
        
        // I/O 서비스 실행을 위한 스레드 풀
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([this]() {
                io_context.run();
            });
        }

        // 메인 스레드에서 사용자 입력 처리
        std::string input;
        while (std::getline(std::cin, input)) {
            if (input == "quit") {
                break;
            }
        }

        // 정상 종료
        io_context.stop();
        for (auto& thread : threads) {
            thread.join();
        }
    }
};

int main() {
    try {
        Server server;
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "에러 발생: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 