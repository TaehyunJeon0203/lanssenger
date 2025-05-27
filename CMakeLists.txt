cmake_minimum_required(VERSION 3.10)
project(lanssenger)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
)

# 서버 링크
target_link_libraries(server
    ${Boost_LIBRARIES}
    pthread
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
    )

    # 콘솔 클라이언트 실행 파일
    add_executable(client
        src/client/main.cpp
        src/client/nickname_manager.cpp
    )
    target_compile_definitions(client PRIVATE CONSOLE_CLIENT)

    # GUI 클라이언트 실행 파일
    add_executable(client_gui
        src/client/gui/main_gui.cpp
        src/client/nickname_manager.cpp
        src/client/gui/mainwindow.cpp
        include/client/gui/mainwindow.hpp
        ${UI_FILES}
    )

    # 클라이언트 링크
    target_link_libraries(client
        ${Boost_LIBRARIES}
        pthread
        Qt6::Core
        Qt6::Widgets
    )

    # GUI 클라이언트 링크
    target_link_libraries(client_gui
        ${Boost_LIBRARIES}
        pthread
        Qt6::Core
        Qt6::Widgets
    )
endif()

# 빌드 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) 