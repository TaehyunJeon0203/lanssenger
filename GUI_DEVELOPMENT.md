# GUI 개발 가이드

## 1. 개발 환경 설정

### 1.1 로컬 Qt Creator 설치

```bash
# Ubuntu/Debian 기준
sudo apt-get update
sudo apt-get install -y \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qtcreator
```

### 1.2 Docker 환경 설정

```bash
# 1. 프로젝트 클론
git clone [프로젝트_주소]
cd lansseneger

# 2. 개발용 Docker 컨테이너 실행
docker-compose up -d
```

### 1.3 로컬 Qt Creator 설정

1. Qt Creator에서 프로젝트 열기

   - File > Open File or Project > CMakeLists.txt 선택
   - Kit 선택: Desktop Qt 6.x.x

2. 빌드 설정

   - Projects > Build Settings
   - Build directory: `build`
   - CMake configuration:
     ```
     -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake
     ```

3. 실행 설정 (Docker에서 실행)
   - Projects > Run Settings
   - Run configuration: Custom Executable
   - Executable: `docker exec -i lanssenger-client-1 /app/client`

### 1.4 로컬 빌드 및 실행

```bash
# 빌드
mkdir -p build && cd build
cmake ..
make

# 실행
./bin/client
```

## 2. 개발 워크플로우

### 2.1 코드 작성

1. 로컬 Qt Creator에서 코드 작성
2. 코드 자동 완성, 문법 검사 등 IDE 기능 활용
3. Qt Designer로 UI 디자인

### 2.2 빌드 및 실행

1. 로컬에서 빌드

   ```bash
   mkdir -p build && cd build
   cmake ..
   make
   ```

2. Docker 컨테이너에 실행 파일 복사

   ```bash
   docker cp build/bin/client lanssenger-client-1:/app/
   ```

3. Docker에서 실행
   ```bash
   docker exec -it lanssenger-client-1 /app/client
   ```

### 2.3 자동화 스크립트

빌드와 실행을 자동화하는 스크립트를 만들어 사용할 수 있습니다:

```bash
#!/bin/bash
# build_and_run.sh

# 로컬 빌드
mkdir -p build && cd build
cmake ..
make

# Docker 컨테이너에 복사
docker cp bin/client lanssenger-client-1:/app/

# Docker에서 실행
docker exec -it lanssenger-client-1 /app/client
```

## 3. 프로젝트 구조

```
project/
├── include/
│   └── client/
│       └── gui/
│           └── mainwindow.hpp    # GUI 클래스 헤더
└── src/
    └── client/
        └── gui/
            └── mainwindow.cpp    # GUI 클래스 구현
```

## 4. 개발 가이드

### 4.1 UI 수정

1. Qt Designer 사용:

   - 로컬 Qt Creator에서 UI 파일 생성/수정
   - 시각적 디자인 도구 활용

2. 코드로 직접 수정:
   ```cpp
   void MainWindow::setupUI() {
       // UI 컴포넌트 생성 및 배치
   }
   ```

### 4.2 이벤트 처리

```cpp
void MainWindow::setupConnections() {
    // 버튼 클릭 이벤트
    connect(sendButton, &QPushButton::clicked,
            this, &MainWindow::sendMessage);

    // 엔터키 이벤트
    connect(messageInput, &QLineEdit::returnPressed,
            this, &MainWindow::sendMessage);
}
```

### 4.3 메시지 처리

```cpp
void MainWindow::sendMessage() {
    QString message = messageInput->text().trimmed();
    if (!message.isEmpty()) {
        // TODO: 메시지 전송 로직 구현
        messageInput->clear();
    }
}

void MainWindow::appendMessage(const QString& message) {
    chatDisplay->append(message);
}
```

## 5. 개발 순서

1. 기본 UI 구현

   - 채팅창 레이아웃
   - 메시지 입력 영역
   - 사용자 목록

2. 이벤트 처리 구현

   - 메시지 전송
   - 사용자 목록 업데이트
   - 연결 상태 표시

3. 백엔드 연동
   - 메시지 송수신
   - 사용자 관리
   - 연결 관리

## 6. 테스트 방법

1. 로컬 빌드:

```bash
mkdir build && cd build
cmake ..
make
```

2. Docker 빌드:

```bash
docker-compose up --build client
```

## 7. 협업 가이드

### 7.1 Git 브랜치 전략

1. 메인 브랜치

   - `main`: 안정적인 릴리스 버전
   - `develop`: 개발 중인 버전

2. 기능 브랜치
   - `feature/gui-xxx`: GUI 기능 개발
   - `feature/backend-xxx`: 백엔드 기능 개발

### 7.2 코드 리뷰

1. Pull Request 생성

   - 기능 브랜치에서 develop 브랜치로 PR 생성
   - 변경사항 설명 작성
   - 리뷰어 지정

2. 코드 리뷰 진행
   - UI/UX 검토
   - 코드 품질 검토
   - 테스트 결과 확인

### 7.3 테스트

1. 로컬 테스트

   - Qt Creator에서 UI 동작 테스트
   - 단위 테스트 실행

2. 통합 테스트
   - Docker 환경에서 서버와 연동 테스트
   - 실제 사용 시나리오 테스트

## 8. 주의사항

1. 메모리 관리

   - Qt의 부모-자식 관계 활용
   - 적절한 소멸자 구현

2. 스레드 안전성

   - UI 업데이트는 메인 스레드에서
   - 시그널/슬롯 활용

3. 에러 처리
   - 네트워크 오류 처리
   - 사용자 피드백 제공

## 9. 추가 기능 제안

1. UI 개선

   - 다크 모드
   - 폰트 크기 조절
   - 이모지 지원

2. 사용자 경험
   - 메시지 알림
   - 타이핑 표시
   - 파일 전송

## 10. 문제 해결

### 10.1 빌드 오류

- CMake 오류: Qt 경로 확인
- 컴파일 오류: 의존성 패키지 설치 확인

### 10.2 런타임 오류

- 메모리 누수: 부모-자식 관계 확인
- 스레드 오류: 시그널/슬롯 사용 확인

### 10.3 UI 오류

- 레이아웃 깨짐: 크기 정책 확인
- 이벤트 미동작: 시그널/슬롯 연결 확인

## 11. 참고 자료

1. Qt 공식 문서

   - [Qt 6 Documentation](https://doc.qt.io/qt-6/)
   - [Qt Widgets](https://doc.qt.io/qt-6/widgets.html)

2. 개발 도구

   - [Qt Creator](https://www.qt.io/product/development-tools)
   - [Qt Designer](https://doc.qt.io/qt-6/qtdesigner-manual.html)

3. 커뮤니티
   - [Qt Forum](https://forum.qt.io/)
   - [Stack Overflow Qt 태그](https://stackoverflow.com/questions/tagged/qt)

## 12. 개발 팁

### 12.1 로컬 개발 장점

1. 빠른 코드 작성

   - 자동 완성
   - 문법 검사
   - 리팩토링 도구

2. 디버깅 용이

   - 중단점 설정
   - 변수 검사
   - 호출 스택 확인

3. UI 디자인 편의

   - Qt Designer 실시간 미리보기
   - 드래그 앤 드롭으로 UI 구성

### 12.2 Docker 실행 장점

1. 환경 일관성

   - 모든 개발자가 동일한 환경
   - 의존성 문제 해결

2. 배포 용이

   - 개발 환경과 동일한 실행 환경
   - 컨테이너화된 애플리케이션

### 12.3 자주 사용하는 명령어

```bash
# Docker 컨테이너 상태 확인
docker ps

# 로그 확인
docker logs lanssenger-client-1

# 컨테이너 재시작
docker-compose restart client

# 실행 파일 복사
docker cp build/bin/client lanssenger-client-1:/app/
```
