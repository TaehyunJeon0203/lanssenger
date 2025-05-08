#pragma once
#include <string>
#include <cstdint>

// 네트워크 설정
#define SERVER_PORT 12345
#define MAX_BUFFER 1024
#define MAX_CLIENTS 10

// 메시지 타입
enum class MessageType : uint8_t {
    CONNECT,    // 연결 요청
    DISCONNECT, // 연결 해제
    CHAT,       // 일반 채팅
    SYSTEM      // 시스템 메시지
};

// 메시지 구조체
struct Message {
    MessageType type;
    char sender[32];
    char content[MAX_BUFFER - 32 - 1]; // sender 크기를 제외한 나머지 공간
};

// 에러 코드
enum class ErrorCode {
    NONE,
    CONNECTION_FAILED,
    SEND_FAILED,
    RECEIVE_FAILED
};
