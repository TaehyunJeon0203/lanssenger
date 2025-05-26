#include "client/gui/mainwindow.hpp"
#include <QMessageBox>
#include <QStatusBar>
#include <QInputDialog>

// ChatClient 클래스 정의를 포함
#include "../main.cpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();
    connectToServer();
}

MainWindow::~MainWindow()
{
    if (chatClient) {
        chatClient->stop();
    }
}

void MainWindow::setupUI()
{
    // 중앙 위젯 설정
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 메인 레이아웃
    mainLayout = new QVBoxLayout(centralWidget);
    
    // 채팅 표시 영역
    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);
    mainLayout->addWidget(chatDisplay);
    
    // 메시지 입력 영역
    QHBoxLayout* inputLayout = new QHBoxLayout();
    messageInput = new QLineEdit(this);
    sendButton = new QPushButton("전송", this);
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);
    mainLayout->addLayout(inputLayout);
    
    // 사용자 목록
    userList = new QListWidget(this);
    userList->setMaximumWidth(200);
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->addWidget(chatDisplay);
    contentLayout->addWidget(userList);
    mainLayout->addLayout(contentLayout);
    
    // 상태 표시줄
    statusLabel = new QLabel("연결 중...", this);
    statusBar()->addWidget(statusLabel);
    
    // 윈도우 설정
    setWindowTitle("Lanssenger Chat");
    resize(800, 600);
}

void MainWindow::setupConnections()
{
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
}

void MainWindow::connectToServer()
{
    // 닉네임 입력 받기
    bool ok;
    QString nickname = QInputDialog::getText(this, "닉네임 입력",
                                           "사용할 닉네임을 입력하세요:",
                                           QLineEdit::Normal,
                                           "", &ok);
    if (!ok || nickname.isEmpty()) {
        QMessageBox::critical(this, "오류", "닉네임을 입력해야 합니다.");
        close();
        return;
    }

    // ChatClient 생성 및 서버 연결
    chatClient = std::make_unique<ChatClient>();
    if (!chatClient->connect("localhost", 8080)) {
        QMessageBox::critical(this, "연결 오류",
                            "서버에 연결할 수 없습니다.");
        close();
        return;
    }

    // 메시지 수신 시 UI 업데이트
    chatClient->setMessageCallback([this](const std::string& message) {
        appendMessage(QString::fromStdString(message));
    });

    // 수신 스레드 시작
    chatClient->start();
    statusLabel->setText("연결됨");
}

void MainWindow::sendMessage()
{
    QString message = messageInput->text().trimmed();
    if (!message.isEmpty()) {
        chatClient->sendMessage(message.toStdString());
        messageInput->clear();
    }
}

void MainWindow::appendMessage(const QString& message)
{
    chatDisplay->append(message);
}

void MainWindow::updateUserList(const QStringList& users)
{
    userList->clear();
    userList->addItems(users);
} 