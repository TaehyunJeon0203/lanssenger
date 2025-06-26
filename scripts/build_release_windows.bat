@echo off
setlocal enabledelayedexpansion

REM 0. build 디렉토리의 exe를 루트로 복사
if exist client_gui.exe del /f /q client_gui.exe
copy build\client_gui.exe .

REM 1. windeployqt로 Qt 의존성 복사 (전체 경로 사용)
echo Qt 의존성 복사 중...
"C:\Qt\6.9.0\msvc2022_64\bin\windeployqt.exe" --qmldir . client_gui.exe

REM 2. QtDBus 관련 DLL 복사
if exist "C:\Qt\6.9.0\msvc2022_64\bin\Qt6DBus.dll" (
    copy "C:\Qt\6.9.0\msvc2022_64\bin\Qt6DBus.dll" .
    copy "C:\Qt\6.9.0\msvc2022_64\bin\Qt6DBusd.dll" .
)

REM 3. Boost 관련 DLL 복사
if exist "C:\local\boost_1_85_0\lib\boost_system-vc143-mt-x64-1_85.dll" (
    copy "C:\local\boost_1_85_0\lib\boost_system-vc143-mt-x64-1_85.dll" .
)

REM 4. MSVC 런타임 DLL 복사 (2022 기준 경로 확인 필요)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.34.31933\x64\Microsoft.VC143.CRT\msvcp140.dll" (
    copy "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.34.31933\x64\Microsoft.VC143.CRT\msvcp140.dll" .
    copy "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.34.31933\x64\Microsoft.VC143.CRT\vcruntime140.dll" .
    copy "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.34.31933\x64\Microsoft.VC143.CRT\vcruntime140_1.dll" .
)

REM 5. 기존 zip 삭제 후 새로 압축
if exist client_gui.zip del /f /q client_gui.zip
powershell Compress-Archive -Path client_gui.exe,*.dll,platforms,styles,imageformats -DestinationPath client_gui.zip

echo ✅ client_gui.zip 파일이 준비되었습니다! 사용자는 압축 해제 후 바로 실행하면 됩니다.
