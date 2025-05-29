# 윈도우 빌드 가이드

## 필수 요구사항

- Docker Desktop
- CMake
- Boost 라이브러리
- Qt6

## 빌드 및 실행 방법

### 1. 서버 실행

```cmd
docker-compose up -d
```

### 2. 클라이언트 빌드

```cmd
scripts\build_client.bat
```

### 3. 클라이언트 실행

```cmd
# GUI 클라이언트 실행
.\build\client_gui.exe

# 콘솔 클라이언트 실행 (선택사항)
.\build\client.exe
```
