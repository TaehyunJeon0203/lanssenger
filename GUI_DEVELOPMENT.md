# GUI 개발 가이드

## 1. 개발 환경 설정

### 1.1 필수 도구 설치

```bash
# Ubuntu/Debian 기준
sudo apt-get update
sudo apt-get install -y \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qtcreator \
    git
```

### 1.2 Qt Creator 설정

1. Qt Creator 실행
2. Tools > Options > Kits에서 Qt 6 버전 선택
3. Tools > Options > Build & Run에서 컴파일러 설정

## 2. 프로젝트 구조

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

## 3. 주요 클래스 설명

### 3.1 MainWindow 클래스

```cpp
class MainWindow : public QMainWindow {
    // UI 컴포넌트
    QTextEdit* chatDisplay;    // 채팅 메시지 표시
    QLineEdit* messageInput;   // 메시지 입력
    QPushButton* sendButton;   // 전송 버튼
    QListWidget* userList;     // 사용자 목록
    QLabel* statusLabel;       // 상태 표시
};
```

### 3.2 주요 기능

1. 메시지 전송/수신
2. 사용자 목록 관리
3. 연결 상태 표시

## 4. 개발 가이드

### 4.1 UI 수정

1. Qt Designer 사용:
   ```bash
   # UI 파일 생성
   qtcreator mainwindow.ui
   ```
2. 또는 코드로 직접 수정:
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

1. Git 브랜치 관리:

   - `feature/gui-xxx` 형식으로 브랜치 생성
   - PR을 통한 코드 리뷰

2. 코드 스타일:
   - Qt 코딩 컨벤션 준수
   - 일관된 들여쓰기 사용
   - 적절한 주석 작성

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
