# LANssenger GUI 클라이언트 개발 및 테스트 가이드

## 목차

- [1. 개요](#1-개요)
- [2. macOS 개발 및 테스트](#2-macos-개발-및-테스트)
- [3. Windows 개발 및 테스트](#3-windows-개발-및-테스트)
- [4. 공통 개발 워크플로우](#4-공통-개발-워크플로우)
- [5. 참고사항](#5-참고사항)

---

## 1. 개요

- 이 문서는 LANssenger의 **GUI 클라이언트**를 개발하고 테스트하는 방법을 정리한 가이드입니다.
- 기능(로직) 개발자와 GUI 개발자가 분리되어 협업할 수 있도록, 콘솔/GUI 클라이언트가 각각 독립적으로 빌드/실행됩니다.

---

## 2. macOS 개발 및 테스트

### 2.1. 의존성 설치

Homebrew를 사용하여 필수 패키지를 설치합니다.

```bash
brew install boost qt@6 cmake
```

### 2.2. 빌드

```bash
./scripts/build_client.sh
```

### 2.3. 실행

- **콘솔 클라이언트:**
  ```bash
  ./build/client
  ```
- **GUI 클라이언트:**
  ```bash
  ./build/client_gui
  ```

### 2.4. 주의사항

- Qt GUI 실행 시, Xcode Command Line Tools가 필요할 수 있습니다.
- Qt Creator를 사용하면 UI 디자인 및 디버깅이 편리합니다.

---

## 3. Windows 개발 및 테스트

### 3.1. 의존성 설치

- [MSYS2](https://www.msys2.org/) 또는 [vcpkg](https://vcpkg.io/)를 추천합니다.
- 예시: MSYS2 환경에서
  ```bash
  pacman -S mingw-w64-x86_64-boost mingw-w64-x86_64-qt6-base mingw-w64-x86_64-cmake
  ```
- 또는 vcpkg 사용 시
  ```bash
  vcpkg install boost qt6-base
  ```

### 3.2. 빌드

- MSYS2 MinGW 64-bit 터미널에서:
  ```bash
  mkdir build && cd build
  cmake -G "MinGW Makefiles" ..
  mingw32-make
  ```

### 3.3. 실행

- **콘솔 클라이언트:**
  ```bash
  ./client.exe
  ```
- **GUI 클라이언트:**
  ```bash
  ./client_gui.exe
  ```

### 3.4. 주의사항

- Qt DLL 경로가 PATH에 포함되어야 GUI가 정상 실행됩니다.
- Qt Creator를 사용하면 UI 디자인 및 디버깅이 편리합니다.

---

## 4. 공통 개발 워크플로우

1. **기능 개발자**
   - `src/client/main.cpp`에서 콘솔 기반 채팅 로직 개발/테스트
   - `./build/client`로 실행
2. **GUI 개발자**
   - `src/client/gui/mainwindow.cpp`, `mainwindow.hpp` 등에서 UI/UX 개발
   - `src/client/gui/main_gui.cpp`에서 Qt 진입점(main 함수) 관리
   - `./build/client_gui`(mac) 또는 `./client_gui.exe`(win)로 실행
3. **서버는 Docker로 실행**
   - `docker-compose up`으로 서버 실행
   - 클라이언트는 로컬에서 독립적으로 개발/테스트

---

## 5. 참고사항

- 현재 GUI는 UI 뼈대만 구현되어 있으며, 네트워크/채팅 기능은 추가 개발 필요
- Qt Creator, Visual Studio, CLion 등 다양한 IDE에서 개발 가능
- 협업 시 main.cpp(콘솔)와 main_gui.cpp(GUI)를 분리하여 충돌 최소화
- 추가적인 빌드/실행 자동화 스크립트는 필요에 따라 작성

---

문의/협업 관련 사항은 README 또는 팀원에게 문의하세요.
