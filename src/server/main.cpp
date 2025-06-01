#include <iostream>
#include "server/server.hpp"
#include <csignal>
#include <thread>
#include <chrono>

using namespace lanssenger;

Server* server_ptr = nullptr;

void signalHandler(int signum) {
    if (server_ptr) {
        std::cout << "\nReceived signal " << signum << ", shutting down server..." << std::endl;
        server_ptr->stop();
    }
}

int main() {
    // 시그널 핸들러 설정
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 서버 인스턴스 생성
    Server server;
    server_ptr = &server;

    // 서버 시작 (포트 8080)
    if (!server.start(8080)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "Server is running on port 8080. Press Ctrl+C to stop." << std::endl;

    // 메인 스레드가 종료되지 않도록 대기
    while (server.isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
} 