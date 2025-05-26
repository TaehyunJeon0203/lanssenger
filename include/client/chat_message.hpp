#pragma once

#include <string>
#include <chrono>

class ChatMessage {
public:
    enum class Type {
        TEXT,
        SYSTEM,
        ERROR
    };

    ChatMessage(const std::string& sender, 
                const std::string& content, 
                Type type = Type::TEXT);

    // Getters
    const std::string& getSender() const { return sender_; }
    const std::string& getContent() const { return content_; }
    Type getType() const { return type_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }

    // 메시지 포맷팅
    std::string format() const;

private:
    std::string sender_;
    std::string content_;
    Type type_;
    std::chrono::system_clock::time_point timestamp_;
}; 