cmake_minimum_required(VERSION 3.16)
project(lansseneger VERSION 1.0.0 LANGUAGES CXX)

# 빌드 출력 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# C++17 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 설정
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Network REQUIRED)

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

# 클라이언트 라이브러리
add_library(client_lib
    src/client/client.cpp
)

target_include_directories(client_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(client_lib
    PRIVATE
        Qt6::Core
        Qt6::Network
)

# 서버 라이브러리
add_library(server_lib
    src/server/server.cpp
)

target_include_directories(server_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server_lib
    PRIVATE
        Qt6::Core
        Qt6::Network
)

# 클라이언트 실행 파일
add_executable(client
    src/client/main.cpp
)

target_link_libraries(client
    PRIVATE
        client_lib
)

# 서버 실행 파일
add_executable(server
    src/server/main.cpp
)

target_link_libraries(server
    PRIVATE
        server_lib
)

# 설치 설정
install(TARGETS client server
    RUNTIME DESTINATION bin
) 