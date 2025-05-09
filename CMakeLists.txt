cmake_minimum_required(VERSION 3.15)
project(LANssenger VERSION 1.0.0 LANGUAGES CXX)

# 빌드 출력 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# C++17 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 설정
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Boost 설정
find_package(Boost REQUIRED COMPONENTS system)

# 소스 파일 설정
set(CLIENT_SOURCES
    src/client/client.cpp
    src/client/gui/mainwindow.cpp
)

set(SERVER_SOURCES
    src/server/server.cpp
)

# 헤더 파일 설정
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 클라이언트 실행 파일
add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Boost::system
)

# 서버 실행 파일
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE
    Qt6::Core
    Qt6::Network
    Boost::system
)

# 설치 설정
install(TARGETS client server
    RUNTIME DESTINATION bin
) 