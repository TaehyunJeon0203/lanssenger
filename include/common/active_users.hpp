#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

namespace lanssenger {

struct UserConnectionInfo {
    std::string nickname;
    std::string ipLastThree;  // IP 주소의 마지막 3자리
};

class ActiveUsersManager {
public:
    static ActiveUsersManager& getInstance();

    void addUser(const std::string& userId, const std::string& nickname, const std::string& ipAddress);
    void removeUser(const std::string& userId);
    void updateNickname(const std::string& userId, const std::string& nickname);
    std::string getNickname(const std::string& userId) const;
    
    std::unordered_map<std::string, UserConnectionInfo> getAllActiveUsers() const;
    bool isUserActive(const std::string& userId) const;

private:
    ActiveUsersManager() = default;
    ~ActiveUsersManager() = default;
    ActiveUsersManager(const ActiveUsersManager&) = delete;
    ActiveUsersManager& operator=(const ActiveUsersManager&) = delete;

    std::unordered_map<std::string, UserConnectionInfo> activeUsers_;
    mutable std::mutex mutex_;
};

} // namespace lanssenger 