#include "client/gui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QInputDialog>

// ChatClient 클래스 정의를 포함
#include "../main.cpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupConnections();
    connectToServer();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (chatClient) {
        chatClient->stop();
    }
}

void MainWindow::setupConnections()
{
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(ui->messageInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    connect(ui->userListButton, &QPushButton::clicked, this, &MainWindow::requestUserList);

    // 그룹채팅 기능은 잠시 제외
    // connect(ui->groupChatButton, &QPushButton::clicked, this, &MainWindow::showGroupChat);
}

void MainWindow::connectToServer()
{
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

    chatClient = std::make_unique<ChatClient>();
    if (!chatClient->connect("localhost", 8080)) {
        QMessageBox::critical(this, "연결 오류",
                            "서버에 연결할 수 없습니다.");
        close();
        return;
    }

    chatClient->setMessageCallback([this](const std::string& message) {
        appendMessage(QString::fromStdString(message));
    });

    chatClient->start();
    ui->statusLabel->setText("연결됨");

    // 서버에 닉네임 전송
    std::cout << "[MainWindow] 닉네임 전송 요청: " << nickname.toStdString() << std::endl;
    chatClient->sendMessage("/nickname " + nickname.toStdString());
}

void MainWindow::sendMessage()
{
    QString message = ui->messageInput->text().trimmed();
    if (!message.isEmpty()) {
        chatClient->sendMessage(message.toStdString());
        ui->messageInput->clear();
    }
}

void MainWindow::appendMessage(const QString& message)
{
    std::cout << "[MainWindow] 받은 메시지: " << message.toStdString() << std::endl;
    
    // GUI 업데이트는 메인 스레드에서 실행
    QMetaObject::invokeMethod(this, [this, message]() {
        if (message.startsWith("USER_LIST:")) {
            std::cout << "[MainWindow] 유저 목록 메시지 감지됨" << std::endl;
            QString userListStr = message.mid(QString("USER_LIST:").length());
            QStringList users = userListStr.split(",", Qt::SkipEmptyParts);
            
            std::cout << "[MainWindow] 파싱된 유저 목록: " << users.join(", ").toStdString() << std::endl;
            
            if (!userListWindow) {
                std::cout << "[MainWindow] 새로운 UserListWindow 생성" << std::endl;
                userListWindow = std::make_unique<UserListWindow>(this);
            }
            
            userListWindow->updateUserList(users);
            userListWindow->show();
            userListWindow->raise();
            userListWindow->activateWindow();
        } else {
            ui->chatDisplay->append(message);
        }
    }, Qt::QueuedConnection);
}

void MainWindow::requestUserList()
{
    if (chatClient) {
        chatClient->sendMessage("/users");
    }
}

// 그룹채팅 관련 코드 주석처리 (추후 필요 시 복원)
/*
void MainWindow::showGroupChat()
{
    ui->stackedWidget->setCurrentWidget(ui->groupChatWidget);
}

void MainWindow::showMainChat()
{
    ui->stackedWidget->setCurrentWidget(ui->mainChatWidget);
}
*/
