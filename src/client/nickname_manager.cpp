#include "client/nickname_manager.hpp"
#include <sstream>
#include <algorithm>
#include <regex>
#include <iostream>
#include <boost/asio.hpp>
#include <array>
#include <memory>
#include <string>

NicknameManager& NicknameManager::getInstance() {
    static NicknameManager instance;
    return instance;
}

NicknameManager::NicknameManager() {}

std::string NicknameManager::getLocalIpAddress() {
#if defined(_WIN32)
    std::string cmd = "ipconfig";
#elif defined(__APPLE__)
    std::string cmd = "ipconfig getifaddr en0";
#else
    std::string cmd = "hostname -I";
#endif
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "127.0.0.1";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    // 결과에서 개행 문자 제거
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
#if defined(_WIN32)
    // 윈도우의 경우 ipconfig 결과에서 IPv4 Address만 추출
    auto pos = result.find("IPv4");
    if (pos != std::string::npos) {
        auto ip_start = result.find(":", pos);
        if (ip_start != std::string::npos) {
            result = result.substr(ip_start + 1);
            // 공백 제거
            result.erase(0, result.find_first_not_of(" \t"));
        }
    }
#endif
    return result.empty() ? "127.0.0.1" : result;
}

std::string NicknameManager::getLocalIpAddress(const std::string& interface) {
    return getLocalIpAddress();
}

std::string NicknameManager::generateNickname(const std::string& baseName) {
    return generateNickname(baseName, getLocalIpAddress());
}

std::string NicknameManager::generateNickname(const std::string& baseName, const std::string& ipAddress) {
    std::string suffix = generateSuffix(ipAddress);
    return baseName + "(." + suffix + ")";
}

bool NicknameManager::isValidNickname(const std::string& nickname) {
    return validateNicknameFormat(nickname);
}

bool NicknameManager::isNicknameAvailable(const std::string& nickname) {
    std::lock_guard<std::mutex> lock(nicknameMutex_);
    return activeNicknames_.find(nickname) == activeNicknames_.end();
}

bool NicknameManager::registerNickname(const std::string& nickname) {
    if (!isValidNickname(nickname)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(nicknameMutex_);
    if (!isNicknameAvailable(nickname)) {
        return false;
    }

    activeNicknames_.insert(nickname);
    return true;
}

void NicknameManager::unregisterNickname(const std::string& nickname) {
    std::lock_guard<std::mutex> lock(nicknameMutex_);
    activeNicknames_.erase(nickname);
}

std::string NicknameManager::getBaseName(const std::string& nickname) {
    size_t dotPos = nickname.find_last_of('.');
    if (dotPos == std::string::npos) {
        return nickname;
    }
    return nickname.substr(0, dotPos);
}

std::string NicknameManager::generateSuffix(const std::string& ipAddress) {
    // IP 주소에서 마지막 3자리 숫자를 추출
    std::regex ipPattern(R"(\d+\.\d+\.\d+\.(\d+))");
    std::smatch matches;
    
    if (std::regex_match(ipAddress, matches, ipPattern) && matches.size() > 1) {
        std::string lastOctet = matches[1].str();
        // 3자리로 맞추기
        while (lastOctet.length() < 3) {
            lastOctet = "0" + lastOctet;
        }
        return lastOctet;
    }
    
    // IP 주소 파싱 실패 시 기본값 반환
    return "000";
}

bool NicknameManager::validateNicknameFormat(const std::string& nickname) {
    // 닉네임 형식 검증: baseName.xxx (xxx는 3자리 숫자)
    std::regex nicknamePattern(R"(^[a-zA-Z가-힣]+\.\d{3}$)");
    return std::regex_match(nickname, nicknamePattern);
}

void NicknameManager::setCurrentUserNickname(const std::string& nickname) {
    nicknameMutex_.lock();
    currentUserNickname_ = nickname;
    nicknameMutex_.unlock();
}

std::string NicknameManager::getCurrentUserNickname() const {
    nicknameMutex_.lock();
    std::string nickname = currentUserNickname_;
    nicknameMutex_.unlock();
    return nickname;
}

void NicknameManager::updateUserList(const std::vector<std::string>& users) {
    nicknameMutex_.lock();
    userList_ = users;
    if (userListCallback_) {
        userListCallback_(users);
    }
    nicknameMutex_.unlock();
}

std::vector<std::string> NicknameManager::getUserList() const {
    nicknameMutex_.lock();
    std::vector<std::string> users = userList_;
    nicknameMutex_.unlock();
    return users;
}

int NicknameManager::getUserCount() const {
    nicknameMutex_.lock();
    int count = userList_.size();
    nicknameMutex_.unlock();
    return count;
} 