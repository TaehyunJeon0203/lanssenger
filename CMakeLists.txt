cmake_minimum_required(VERSION 3.10)

# vcpkg 설정 추가
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(lanssenger LANGUAGES CXX)  # CXX 명시

if(WIN32)
  set(THREADS_PREFER_PTHREAD_FLAG OFF)
endif()

find_package(Threads REQUIRED)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC에서 Qt C++17 관련 오류 방지
if(MSVC)
  add_compile_options(/Zc:__cplusplus)
endif()

# Windows 타겟 버전 정의 (경고 방지용)
add_compile_definitions(_WIN32_WINNT=0x0601)  # Windows 7 이상 타겟

# Boost 설정
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# 헤더 파일 디렉토리 추가
include_directories(${CMAKE_SOURCE_DIR}/include)

# Boost 라이브러리 찾기
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

# 서버 실행 파일
add_executable(server
    src/server/main.cpp
    src/server/server.cpp
    src/common/active_users.cpp
    src/common/chat_room.cpp
)

target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(server PRIVATE
    Boost::system
    Threads::Threads
)

# 클라이언트 관련 설정 (서버 빌드 시에는 건너뜀)
if(NOT DEFINED BUILD_SERVER_ONLY)
    # Qt6 설정
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    # Qt6 라이브러리 찾기
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    # UI 파일
    set(UI_FILES
        src/client/gui/mainwindow.ui
        src/client/gui/createRoom.ui
        src/client/gui/groupchatwindow.ui
    )

    # GUI 클라이언트 실행 파일
    add_executable(client_gui MACOSX_BUNDLE
        src/client/gui/main_gui.cpp
        src/client/nickname_manager.cpp
        src/client/gui/mainwindow.cpp
        src/client/gui/userlistwindow.cpp
        src/client/gui/createRoom.cpp
        src/client/chat_client.cpp
        src/client/gui/groupchatwindow.cpp
        include/client/gui/mainwindow.hpp
        include/client/gui/userlistwindow.hpp
        include/client/gui/createRoom.hpp
        include/client/chat_client.hpp
        include/client/gui/groupchatwindow.hpp
        ${UI_FILES}
    )

    # GUI 클라이언트 링크 - pthread 대신 Threads::Threads 사용
    target_link_libraries(client_gui
        ${Boost_LIBRARIES}
        Threads::Threads
        Qt6::Core
        Qt6::Widgets
    )
endif()

# 빌드 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
