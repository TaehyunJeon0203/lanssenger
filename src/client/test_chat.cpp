#include "client/chat_manager.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void printTestResult(const std::string& testName, bool result) {
    std::cout << testName << ": " << (result ? "성공" : "실패") << std::endl;
}

// 메시지 콜백 함수
void onMessageReceived(const ChatMessage& message) {
    std::cout << message.format() << std::endl;
}

int main() {
    ChatManager chatManager;
    std::cout << "채팅 시스템 테스트를 시작합니다.\n" << std::endl;

    // 콜백 설정
    chatManager.setMessageCallback(onMessageReceived);

    // 1. 일반 메시지 전송 테스트
    bool sendSuccess = chatManager.sendMessage("안녕하세요!");
    printTestResult("일반 메시지 전송 테스트", sendSuccess);

    // 2. 시스템 메시지 전송 테스트
    bool systemSuccess = chatManager.sendSystemMessage("시스템 메시지입니다.");
    printTestResult("시스템 메시지 전송 테스트", systemSuccess);

    // 3. 오류 메시지 전송 테스트
    bool errorSuccess = chatManager.sendErrorMessage("오류가 발생했습니다.");
    printTestResult("오류 메시지 전송 테스트", errorSuccess);

    // 4. 빈 메시지 전송 테스트
    bool emptyMessage = !chatManager.sendMessage("");
    printTestResult("빈 메시지 전송 테스트", emptyMessage);

    // 5. 메시지 히스토리 테스트
    const auto& history = chatManager.getMessageHistory();
    printTestResult("메시지 히스토리 테스트", history.size() == 3);

    // 6. 메시지 수신 테스트
    ChatMessage receivedMessage("홍길동", "안녕하세요!", ChatMessage::Type::TEXT);
    chatManager.onMessageReceived(receivedMessage);
    printTestResult("메시지 수신 테스트", history.size() == 4);

    // 7. 히스토리 초기화 테스트
    chatManager.clearMessageHistory();
    printTestResult("히스토리 초기화 테스트", history.empty());

    std::cout << "\n테스트가 완료되었습니다." << std::endl;
    return 0;
} 