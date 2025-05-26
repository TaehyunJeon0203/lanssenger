cmake_minimum_required(VERSION 3.10)
project(lansseneger)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost 라이브러리 찾기
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

# Qt 찾기
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 헤더 파일 디렉토리 추가
include_directories(${CMAKE_SOURCE_DIR}/include)

# 서버 실행 파일
add_executable(server
    src/server/main.cpp
    src/server/server.cpp
)

# 클라이언트 실행 파일 (GUI)
add_executable(client
    src/client/main.cpp
    src/client/gui/mainwindow.cpp
    src/client/nickname_manager.cpp
    src/client/chat_message.cpp
    src/client/chat_manager.cpp
)

# 채팅 테스트 프로그램
add_executable(test_chat
    src/client/test_chat.cpp
    src/client/chat_message.cpp
    src/client/chat_manager.cpp
)

# Qt 라이브러리 링크
target_link_libraries(client
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${Boost_LIBRARIES}
)

# 서버에 Boost 라이브러리 링크
target_link_libraries(server
    ${Boost_LIBRARIES}
)

# 빌드 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 