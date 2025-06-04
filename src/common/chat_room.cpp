#include "common/chat_room.hpp"
#include <algorithm>

namespace lanssenger {

ChatRoomManager& ChatRoomManager::getInstance() {
    static ChatRoomManager instance;
    return instance;
}

bool ChatRoomManager::createRoom(const std::string& roomName, const std::string& creatorId,
                               bool isPrivate, const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 이미 존재하는 방 이름인지 확인
    if (rooms_.find(roomName) != rooms_.end()) {
        return false;
    }

    // 새 채팅방 생성
    ChatRoomInfo newRoom;
    newRoom.name = roomName;
    newRoom.creator = creatorId;
    newRoom.isPrivate = isPrivate;
    newRoom.password = password;
    newRoom.members.insert(creatorId);

    rooms_[roomName] = newRoom;
    return true;
}

bool ChatRoomManager::deleteRoom(const std::string& roomName) {
    std::lock_guard<std::mutex> lock(mutex_);
    return rooms_.erase(roomName) > 0;
}

bool ChatRoomManager::joinRoom(const std::string& roomName, const std::string& userId,
                             const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = rooms_.find(roomName);
    if (it == rooms_.end()) {
        return false;
    }

    // 비공개 방인 경우 비밀번호 확인
    if (it->second.isPrivate && it->second.password != password) {
        return false;
    }

    // 이미 참여 중인지 확인
    if (it->second.members.find(userId) != it->second.members.end()) {
        return false;
    }

    it->second.members.insert(userId);
    return true;
}

bool ChatRoomManager::leaveRoom(const std::string& roomName, const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = rooms_.find(roomName);
    if (it == rooms_.end()) {
        return false;
    }

    // 참여자가 아닌 경우
    if (it->second.members.find(userId) == it->second.members.end()) {
        return false;
    }

    it->second.members.erase(userId);

    // 방장이 나간 경우 방 삭제
    if (it->second.creator == userId) {
        rooms_.erase(it);
    }

    return true;
}

ChatRoomInfo* ChatRoomManager::getRoomInfo(const std::string& roomName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = rooms_.find(roomName);
    if (it == rooms_.end()) {
        return nullptr;
    }

    return &(it->second);
}

std::vector<std::string> ChatRoomManager::getUserRooms(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> userRooms;

    for (const auto& room : rooms_) {
        if (room.second.members.find(userId) != room.second.members.end()) {
            userRooms.push_back(room.first);
        }
    }

    return userRooms;
}

std::vector<std::string> ChatRoomManager::getAllRooms() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> roomNames;

    for (const auto& room : rooms_) {
        if (!room.second.isPrivate) {  // 공개 방만 반환
            roomNames.push_back(room.first);
        }
    }

    return roomNames;
}

bool ChatRoomManager::sendMessageToRoom(const std::string& roomName, const std::string& senderId,
                                      const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = rooms_.find(roomName);
    if (it == rooms_.end()) {
        return false;
    }

    // 발신자가 방에 참여 중인지 확인
    if (it->second.members.find(senderId) == it->second.members.end()) {
        return false;
    }

    // TODO: 실제 메시지 전송 로직 구현
    // 여기서는 메시지 전송 성공으로 가정
    return true;
}

} // namespace lanssenger 