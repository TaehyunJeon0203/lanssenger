#pragma once

#include "client/chat_message.hpp"
#include <string>
#include <vector>
#include <functional>
#include <mutex>

class ChatManager {
public:
    using MessageCallback = std::function<void(const ChatMessage&)>;

    ChatManager();
    ~ChatManager() = default;

    // 메시지 전송
    bool sendMessage(const std::string& content);
    bool sendSystemMessage(const std::string& content);
    bool sendErrorMessage(const std::string& content);

    // 메시지 수신 처리
    void onMessageReceived(const ChatMessage& message);

    // 메시지 히스토리
    const std::vector<ChatMessage>& getMessageHistory() const;
    void clearMessageHistory();

    // 콜백 설정
    void setMessageCallback(MessageCallback callback);

private:
    std::vector<ChatMessage> messageHistory_;
    MessageCallback messageCallback_;
    std::mutex mutex_;
    std::string currentUser_;

    void addMessage(const ChatMessage& message);
}; 