#pragma once

#include <string>
#include <unordered_set>
#include <mutex>
#include <functional>
#include <vector>
#include <boost/asio.hpp>

class NicknameManager {
public:
    static NicknameManager& getInstance();

    // IP 주소 관련
    std::string getLocalIpAddress();
    std::string getLocalIpAddress(const std::string& interface);

    // 닉네임 생성 및 검증
    std::string generateNickname(const std::string& baseName);
    std::string generateNickname(const std::string& baseName, const std::string& ipAddress);
    bool isValidNickname(const std::string& nickname);
    bool isNicknameAvailable(const std::string& nickname);

    // 닉네임 관리
    bool registerNickname(const std::string& nickname);
    void unregisterNickname(const std::string& nickname);
    std::string getBaseName(const std::string& nickname);

    // 사용자 관리
    void setCurrentUserNickname(const std::string& nickname);
    std::string getCurrentUserNickname() const;
    void updateUserList(const std::vector<std::string>& users);
    std::vector<std::string> getUserList() const;
    int getUserCount() const;

    // 콜백 설정
    using UserListCallback = std::function<void(const std::vector<std::string>&)>;
    void setUserListCallback(UserListCallback callback) { userListCallback_ = callback; }

private:
    NicknameManager();
    ~NicknameManager() = default;

    // 복사 생성자와 대입 연산자 삭제
    NicknameManager(const NicknameManager&) = delete;
    NicknameManager& operator=(const NicknameManager&) = delete;

    std::unordered_set<std::string> activeNicknames_;
    mutable std::mutex nicknameMutex_;
    boost::asio::io_context io_context_;
    std::string currentUserNickname_;
    std::vector<std::string> userList_;
    UserListCallback userListCallback_;

    // IP 주소에서 닉네임 접미사 생성
    std::string generateSuffix(const std::string& ipAddress);
    
    // 닉네임 형식 검증
    bool validateNicknameFormat(const std::string& nickname);
}; 