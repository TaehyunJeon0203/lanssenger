#pragma once

#include <string>
#include <unordered_set>
#include <mutex>
#include <boost/asio.hpp>

class NicknameManager {
public:
    NicknameManager();
    ~NicknameManager() = default;

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

private:
    std::unordered_set<std::string> activeNicknames_;
    std::mutex nicknameMutex_;
    boost::asio::io_context io_context_;

    // IP 주소에서 닉네임 접미사 생성
    std::string generateSuffix(const std::string& ipAddress);
    
    // 닉네임 형식 검증
    bool validateNicknameFormat(const std::string& nickname);
}; 