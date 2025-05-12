#include "client/client.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// LAN 메신저 클라이언트의 메인 함수
// 명령줄 인터페이스를 통해 서버에 연결하고 메시지를 주고받는 기능을 제공
int main(int argc, char *argv[])
{
    // 기본 연결 정보
    std::string host = "localhost";
    unsigned short port = 8080;

    // 명령줄 인자 처리
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = static_cast<unsigned short>(std::stoi(argv[++i]));
        }
    }

    // 클라이언트 객체 생성
    Client client;

    // 시그널 연결
    client.messageReceived.connect([](const std::string& message) {
        std::cout << "Received: " << message << std::endl;
    });

    client.connected.connect([]() {
        std::cout << "Connected to server" << std::endl;
    });

    client.disconnected.connect([]() {
        std::cout << "Disconnected from server" << std::endl;
    });

    client.error.connect([](const std::string& error) {
        std::cerr << "Error: " << error << std::endl;
    });

    // 서버에 연결
    if (!client.connectToServer(host, port)) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }

    // 메시지 입력 루프
    std::string message;
    while (client.isConnected()) {
        std::getline(std::cin, message);
        
        if (message == "quit") {
            break;
        }
        
        if (!client.sendMessage(message)) {
            std::cerr << "Failed to send message" << std::endl;
            break;
        }
    }

    return 0;
} 