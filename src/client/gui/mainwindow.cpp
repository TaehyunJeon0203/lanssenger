#include "client/gui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QMenuBar>
#include <QMenu>
#include <QDebug>
#include "client/gui/userlistwindow.hpp"
#include "../main.cpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setupMenuBar();
    setupConnections();
    connectToServer();
}

MainWindow::~MainWindow() {
    delete ui;
    if (chatClient) {
        chatClient->stop();
    }
}

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu* roomMenu = menuBar->addMenu(tr("채팅방"));
    QAction* roomListAction = roomMenu->addAction(tr("그룹채팅 열기"));
    connect(roomListAction, &QAction::triggered, this, &MainWindow::showGroupChat);

    QMenu* userMenu = menuBar->addMenu(tr("유저"));
    QAction* userListAction = userMenu->addAction(tr("유저 목록"));
    connect(userListAction, &QAction::triggered, this, &MainWindow::requestUserList);
}

void MainWindow::setupConnections() {
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(ui->messageInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    connect(ui->userListButton, &QPushButton::clicked, this, &MainWindow::requestUserList);
    connect(ui->createRoomButton, &QPushButton::clicked, this, &MainWindow::createNewRoom);
    connect(ui->groupChatButton, &QPushButton::clicked, this, &MainWindow::showGroupChat);
    connect(ui->userListButton2, &QPushButton::clicked, this, &MainWindow::requestUserList);
    connect(ui->mainChatButton, &QPushButton::clicked, this, &MainWindow::showMainChat);
}

void MainWindow::connectToServer() {
    bool ok;
    QString nickname = QInputDialog::getText(this, "닉네임 입력", "사용할 닉네임을 입력하세요:", QLineEdit::Normal, "", &ok);
    if (!ok || nickname.isEmpty()) {
        QMessageBox::critical(this, "오류", "닉네임을 입력해야 합니다.");
        close();
        return;
    }

    chatClient = std::make_unique<ChatClient>();
    if (!chatClient->connect("localhost", 8080)) {
        QMessageBox::critical(this, "연결 오류", "서버에 연결할 수 없습니다.");
        close();
        return;
    }

    chatClient->setMessageCallback([this](const std::string& message) {
        appendMessage(QString::fromStdString(message));
    });

    chatClient->start();
    ui->statusLabel->setText("연결됨");
    chatClient->sendMessage("/nickname " + nickname.toStdString());
}

void MainWindow::sendMessage() {
    QString message = ui->messageInput->text().trimmed();
    if (!message.isEmpty()) {
        chatClient->sendMessage(message.toStdString());
        ui->messageInput->clear();
    }
}

void MainWindow::appendMessage(const QString& message) {
    QMetaObject::invokeMethod(this, [this, message]() {
        if (message.startsWith("USER_LIST:")) {
            QStringList users = message.mid(10).split(",", Qt::SkipEmptyParts);
            if (!userListWindow) userListWindow = std::make_unique<UserListWindow>(this);
            userListWindow->updateUserList(users);
            userListWindow->show();
        } else {
            ui->chatDisplay->append(message);
        }
    }, Qt::QueuedConnection);
}

void MainWindow::requestUserList() {
    if (chatClient) chatClient->sendMessage("/users");
}

// 🔽 그룹채팅 슬롯 구현
void MainWindow::showGroupChat() {
    ui->stackedWidget->setCurrentWidget(ui->groupChatWidget);
}

void MainWindow::showMainChat() {
    ui->stackedWidget->setCurrentWidget(ui->mainChatWidget);
}

void MainWindow::createNewRoom() {
    bool ok;
    QString roomName = QInputDialog::getText(this, "새 채팅방", "채팅방 이름을 입력하세요:", QLineEdit::Normal, "", &ok);
    if (ok && !roomName.isEmpty()) {
        chatClient->sendMessage("/create_room " + roomName.toStdString());
        QMessageBox::information(this, "알림", "채팅방 [" + roomName + "] 생성 요청을 전송했습니다.");
    }
}
