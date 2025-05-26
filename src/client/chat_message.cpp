#include "client/chat_message.hpp"
#include <sstream>
#include <iomanip>

ChatMessage::ChatMessage(const std::string& sender, 
                        const std::string& content, 
                        Type type)
    : sender_(sender)
    , content_(content)
    , type_(type)
    , timestamp_(std::chrono::system_clock::now())
{
}

std::string ChatMessage::format() const {
    std::stringstream ss;
    
    // 타임스탬프 포맷팅
    auto time = std::chrono::system_clock::to_time_t(timestamp_);
    ss << std::put_time(std::localtime(&time), "[%H:%M:%S] ");

    // 메시지 타입에 따른 포맷팅
    switch (type_) {
        case Type::SYSTEM:
            ss << "[시스템] " << content_;
            break;
        case Type::ERROR:
            ss << "[오류] " << content_;
            break;
        case Type::TEXT:
        default:
            ss << sender_ << ": " << content_;
            break;
    }

    return ss.str();
} 