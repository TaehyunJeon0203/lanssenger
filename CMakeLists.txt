cmake_minimum_required(VERSION 3.16)
project(lansseneger VERSION 1.0.0 LANGUAGES CXX)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost 라이브러리 찾기
find_package(Boost REQUIRED COMPONENTS system thread)

# 헤더 파일 디렉토리 추가
include_directories(${CMAKE_SOURCE_DIR}/include)

# 서버 실행 파일
add_executable(server
    src/server/main.cpp
    src/server/server.cpp
)

# 클라이언트 실행 파일
add_executable(client
    src/client/main.cpp
    src/client/client.cpp
)

# Boost 라이브러리 링크
target_link_libraries(server PRIVATE
    Boost::system
    Boost::thread
)

target_link_libraries(client PRIVATE
    Boost::system
    Boost::thread
)

# 빌드 디렉토리 설정
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin) 