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
    ui->statusLabel->setText("연결됨");
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
    ui->chatDisplay->append(message);
}

void MainWindow::updateUserList(const QStringList& users)
{
    ui->userList->clear();
    ui->userList->addItems(users);
} 