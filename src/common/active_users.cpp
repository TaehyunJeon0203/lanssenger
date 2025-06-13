#include "common/active_users.hpp"
#include <sstream>
#include <iomanip>

namespace lanssenger {

ActiveUsersManager& ActiveUsersManager::getInstance() {
    static ActiveUsersManager instance;
    return instance;
}

void ActiveUsersManager::addUser(const std::string& userId, const std::string& nickname, const std::string& ipAddress) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // IP 주소의 마지막 점(.)부터 끝까지 추출
    std::string lastThree;
    size_t lastDot = ipAddress.find_last_of('.');
    if (lastDot != std::string::npos) {
        lastThree = ipAddress.substr(lastDot);  // ".134" 형식으로 저장
    } else {
        lastThree = ipAddress;
    }
    
    activeUsers_[userId] = UserConnectionInfo{nickname, lastThree};
}

void ActiveUsersManager::removeUser(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    activeUsers_.erase(userId);
}

std::unordered_map<std::string, UserConnectionInfo> ActiveUsersManager::getAllActiveUsers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeUsers_;
}

bool ActiveUsersManager::isUserActive(const std::string& userId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeUsers_.find(userId) != activeUsers_.end();
}

void ActiveUsersManager::updateNickname(const std::string& userId, const std::string& nickname) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (activeUsers_.find(userId) != activeUsers_.end()) {
        activeUsers_[userId].nickname = nickname;
    }
}

std::string ActiveUsersManager::getNickname(const std::string& userId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = activeUsers_.find(userId);
    if (it != activeUsers_.end()) {
        return it->second.nickname;
    }
    return userId;  // 사용자를 찾을 수 없는 경우 userId 반환
}
} // namespace lanssenger 