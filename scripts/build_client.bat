@echo off
REM 기존 빌드 디렉토리 삭제 (있을 경우)
if exist build (
    rd /s /q build
)

REM 빌드 디렉토리 생성
mkdir build
cd build

REM MSVC 환경변수 설정 (필수!)
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

REM Ninja + MSVC로 CMake 빌드 설정
cmake .. ^
 -G Ninja ^
 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
 -DQt6_DIR="C:/Qt/6.9.0/msvc2022_64/lib/cmake/Qt6" ^
 -DCMAKE_THREAD_LIBS_INIT=""

REM 빌드 실행
ninja client client_gui

echo 클라이언트 빌드가 완료되었습니다.
echo 콘솔 버전 실행: .\build\client.exe
echo GUI 버전 실행: .\build\client_gui.exe
