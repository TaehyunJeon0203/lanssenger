#include "server/server.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// LAN 메신저 서버의 메인 함수
// 명령줄 인터페이스를 통해 서버를 시작하고 클라이언트의 연결을 관리
int main(int argc, char *argv[])
{
    // 기본 포트 설정
    unsigned short port = 8080;

    // 명령줄 인자 처리
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            port = static_cast<unsigned short>(std::stoi(argv[++i]));
        }
    }

    // 서버 객체 생성
    Server server;

    // 시그널 연결
    server.clientConnected.connect([](const std::string& clientId) {
        std::cout << "Client connected: " << clientId << std::endl;
    });

    server.clientDisconnected.connect([](const std::string& clientId) {
        std::cout << "Client disconnected: " << clientId << std::endl;
    });

    server.messageReceived.connect([](const std::string& clientId, const std::string& message) {
        std::cout << "Message from " << clientId << ": " << message << std::endl;
    });

    server.error.connect([](const std::string& error) {
        std::cerr << "Error: " << error << std::endl;
    });

    // 서버 시작
    if (!server.start(port)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "Server started on port " << port << std::endl;
    std::cout << "Press Enter to stop the server..." << std::endl;

    // 서버 종료 대기
    std::cin.get();

    return 0;
} 