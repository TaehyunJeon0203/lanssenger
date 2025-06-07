#!/bin/bash

# 빌드 디렉토리 생성
mkdir -p build

# CMake로 빌드
cd build
cmake ..
cmake --build . --target client_gui

# 실행 파일에 실행 권한 부여
chmod +x client_gui

echo "클라이언트 빌드가 완료되었습니다."
echo "GUI 버전 실행: ./build/client_gui" 