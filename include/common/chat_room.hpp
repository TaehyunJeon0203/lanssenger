#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <vector>

namespace lanssenger {

// 채팅방 정보를 담는 구조체
struct ChatRoomInfo {
    std::string name;           // 채팅방 이름
    std::unordered_set<std::string> members;  // 참여자 ID 목록
    bool isPrivate;            // 비공개 방 여부
    std::string password;      // 비공개 방 비밀번호 (비공개 방인 경우에만 사용)
};

class ChatRoomManager {
public:
    static ChatRoomManager& getInstance();

    // 채팅방 생성
    bool createRoom(const std::string& roomName, bool isPrivate = false, const std::string& password = "");

    // 채팅방 삭제
    bool deleteRoom(const std::string& roomName);

    // 채팅방 참여
    bool joinRoom(const std::string& roomName, const std::string& userId, 
                 const std::string& password);

    // 채팅방 퇴장
    bool leaveRoom(const std::string& roomName, const std::string& userId);

    // 채팅방 정보 조회
    ChatRoomInfo* getRoomInfo(const std::string& roomName);
    
    // 사용자가 참여 중인 채팅방 목록 조회
    std::vector<std::string> getUserRooms(const std::string& userId);
    
    // 모든 채팅방 목록 조회
    std::vector<std::string> getAllRooms();

    // 공개 채팅방 목록 조회
    std::vector<std::string> getPublicRooms();

    // 비공개 채팅방 목록 조회
    std::vector<std::string> getPrivateRooms();

    // 채팅방에 메시지 전송
    bool sendMessageToRoom(const std::string& roomName, const std::string& senderId, 
                          const std::string& message);

private:
    ChatRoomManager() = default;
    ~ChatRoomManager() = default;
    ChatRoomManager(const ChatRoomManager&) = delete;
    ChatRoomManager& operator=(const ChatRoomManager&) = delete;

    std::unordered_map<std::string, ChatRoomInfo> rooms_;
    mutable std::mutex mutex_;
};

} // namespace lanssenger 