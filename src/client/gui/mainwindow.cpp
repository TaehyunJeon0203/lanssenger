#include "client/gui/mainwindow.hpp"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();
}

MainWindow::~MainWindow()
{
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
    statusLabel = new QLabel("연결됨", this);
    statusBar()->addWidget(statusLabel);
    
    // 윈도우 설정
    setWindowTitle("Lansseneger Chat");
    resize(800, 600);
}

void MainWindow::setupConnections()
{
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
}

void MainWindow::sendMessage()
{
    QString message = messageInput->text().trimmed();
    if (!message.isEmpty()) {
        // TODO: 메시지 전송 로직 구현
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