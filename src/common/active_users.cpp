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
    
    // IP 주소의 마지막 3자리 추출
    std::string lastThree;
    size_t lastDot = ipAddress.find_last_of('.');
    if (lastDot != std::string::npos) {
        lastThree = ipAddress.substr(lastDot + 1);
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
} // namespace lanssenger 