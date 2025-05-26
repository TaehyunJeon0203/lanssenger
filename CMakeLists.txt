cmake_minimum_required(VERSION 3.10)
project(lanssenger)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 헤더 파일 디렉토리 추가
include_directories(${CMAKE_SOURCE_DIR}/include)

# Boost 라이브러리 찾기
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

# 서버 실행 파일
add_executable(server
    src/server/main.cpp
    src/server/server.cpp
)

# 클라이언트 실행 파일
add_executable(client
    src/client/main.cpp
    src/client/nickname_manager.cpp
    src/client/chat_message.cpp
    src/client/chat_manager.cpp
)

# 서버 링크
target_link_libraries(server
    ${Boost_LIBRARIES}
    pthread
)

# 클라이언트 링크
target_link_libraries(client
    ${Boost_LIBRARIES}
    pthread
)

# 빌드 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) 