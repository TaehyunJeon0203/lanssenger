@echo off
REM 빌드 디렉토리 생성
mkdir build

REM CMake로 빌드
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
    -DBOOST_ROOT="C:/Program Files/boost" ^
    -DBoost_INCLUDE_DIR="C:/Program Files/boost/include" ^
    -DBoost_LIBRARY_DIR="C:/Program Files/boost/lib"
cmake --build . --target client client_gui

echo 클라이언트 빌드가 완료되었습니다.
echo 콘솔 버전 실행: .\build\client.exe
echo GUI 버전 실행: .\build\client_gui.exe 