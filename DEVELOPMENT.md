# LAN Messenger 개발 가이드

## 개발 환경 설정

### 1. 필수 요구사항

- Docker
- Docker Compose
- Git

### 2. GUI 애플리케이션 실행을 위한 추가 요구사항

- macOS: XQuartz 설치
- Windows: X Server 설치
- Linux: 기본 지원

## 개발 시작하기

### 1. 개발 환경 실행

```bash
# 도커 컨테이너 빌드 및 실행
docker-compose up --build -d
```

### 2. 개발 컨테이너 접속

```bash
# 컨테이너에 접속
docker-compose exec dev bash
```

### 3. 빌드

```bash
# 빌드 디렉토리로 이동
cd build

# CMake 실행
cmake ..

# 빌드 실행
make
```

## 개발 워크플로우

1. 로컬에서 코드 수정

   - `src/` 디렉토리: 소스 코드
   - `include/` 디렉토리: 헤더 파일

2. 코드 수정 후 빌드

   ```bash
   # 컨테이너 내부에서
   cd build
   make
   ```

3. 실행 파일 위치
   - 빌드된 실행 파일은 `build/bin/` 디렉토리에 생성됨
   - `client`: 클라이언트 실행 파일
   - `server`: 서버 실행 파일

## 프로젝트 구조

```
.
├── CMakeLists.txt              # 빌드 설정
├── Dockerfile                  # 개발용 도커 설정
├── Dockerfile.prod            # 프로덕션용 도커 설정
├── docker-compose.yml         # 개발용 도커 컴포즈
├── docker-compose.prod.yml    # 프로덕션용 도커 컴포즈
├── include/                   # 헤더 파일들
│   ├── client/
│   │   └── gui/              # GUI 관련 헤더
│   └── server/
├── src/                      # 소스 파일들
│   ├── client/
│   │   └── gui/             # GUI 관련 소스
│   └── server/
└── README.md
```

## 유용한 명령어

### 도커 관련

```bash
# 개발 환경 시작
docker-compose up -d

# 개발 환경 중지
docker-compose down

# 로그 확인
docker-compose logs -f

# 컨테이너 재시작
docker-compose restart
```

### 빌드 관련

```bash
# 전체 재빌드
cd build
cmake ..
make

# 클린 빌드
cd build
rm -rf *
cmake ..
make
```

## 문제 해결

### GUI 관련 문제

1. X11 포워딩 오류

   - XQuartz/X Server가 실행 중인지 확인
   - DISPLAY 환경 변수 설정 확인

2. 권한 문제
   - X11 소켓 권한 확인
   - `xhost +` 명령어로 접근 허용

### 빌드 관련 문제

1. CMake 오류

   - CMake 캐시 삭제 후 재시도
   - 의존성 패키지 설치 확인

2. 컴파일 오류
   - 컴파일러 버전 확인
   - 의존성 라이브러리 버전 확인
