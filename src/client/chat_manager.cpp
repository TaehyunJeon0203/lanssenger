#include "client/chat_manager.hpp"

ChatManager::ChatManager() {}

bool ChatManager::sendMessage(const std::string& content) {
    if (content.empty()) {
        return false;
    }

    ChatMessage message(currentUser_, content, ChatMessage::Type::TEXT);
    addMessage(message);
    return true;
}

bool ChatManager::sendSystemMessage(const std::string& content) {
    if (content.empty()) {
        return false;
    }

    ChatMessage message("", content, ChatMessage::Type::SYSTEM);
    addMessage(message);
    return true;
}

bool ChatManager::sendErrorMessage(const std::string& content) {
    if (content.empty()) {
        return false;
    }

    ChatMessage message("", content, ChatMessage::Type::ERROR);
    addMessage(message);
    return true;
}

void ChatManager::onMessageReceived(const ChatMessage& message) {
    addMessage(message);
}

const std::vector<ChatMessage>& ChatManager::getMessageHistory() const {
    return messageHistory_;
}

void ChatManager::clearMessageHistory() {
    std::lock_guard<std::mutex> lock(mutex_);
    messageHistory_.clear();
}

void ChatManager::setMessageCallback(MessageCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    messageCallback_ = std::move(callback);
}

void ChatManager::addMessage(const ChatMessage& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    messageHistory_.push_back(message);
    
    if (messageCallback_) {
        messageCallback_(message);
    }
} 