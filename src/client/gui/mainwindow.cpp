#include "client/gui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QMenuBar>
#include <QMenu>
#include <QDebug>
#include <QSettings>
#include <QListWidgetItem> 
#include "client/gui/userlistwindow.hpp"
#include "client/gui/createRoom.hpp"
#include "client/chat_client.hpp"
#include "client/gui/groupchatwindow.hpp"
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->chatDisplay->setReadOnly(true);
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
    connect(ui->roomListWidget, &QListWidget::itemClicked, this, &MainWindow::joinSelectedRoom);
}

void MainWindow::connectToServer() {
    QSettings settings;
    QString savedNickname = settings.value("nickname").toString();
    bool ok;

    QString nickname;
    if (savedNickname.isEmpty()) {
        nickname = QInputDialog::getText(this, "닉네임 입력", "사용할 닉네임을 입력하세요:", QLineEdit::Normal, "", &ok);
        if (!ok || nickname.isEmpty()) {
            QMessageBox::critical(this, "오류", "닉네임을 입력해야 합니다.");
            close();
            return;
        }
        // 닉네임 저장
        settings.setValue("nickname", nickname);
    } else {
        nickname = QInputDialog::getText(this, "닉네임 입력", "사용할 닉네임을 입력하세요:", QLineEdit::Normal, savedNickname, &ok);
        if (!ok) {
            close();
            return;
        }
        if (nickname != savedNickname) {
            // 닉네임이 변경된 경우 저장
            settings.setValue("nickname", nickname);
        }
    }

    QString serverIp = QInputDialog::getText(this, "서버 IP 입력", "서버 IP 주소를 입력하세요:", QLineEdit::Normal, "localhost", &ok);
    if (!ok) {
        QMessageBox::critical(this, "오류", "서버 IP를 입력해야 합니다.");
        close();
        return;
    }

    chatClient = std::make_unique<ChatClient>();
    if (!chatClient->connect(serverIp.toStdString(), 8080)) {
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
            if (!userListWindow) {
                userListWindow = std::make_unique<UserListWindow>(this);
            }
            userListWindow->updateUserList(users);
            userListWindow->show();
            userListWindow->raise();
            userListWindow->activateWindow();
        } else if (message.startsWith("ROOM_LIST:")) {
            QStringList rooms = message.mid(10).split(",", Qt::SkipEmptyParts);
            ui->roomListWidget->clear();
            ui->roomListWidget->addItems(rooms);
        } else if (message.startsWith("ROOM_MSG:")) {
            QString roomMsg = message.mid(9);
            if (groupChatWindow) {
                groupChatWindow->appendMessage(roomMsg);
            }
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
    if (chatClient) {
        chatClient->sendMessage("/list_rooms");
    }
    ui->stackedWidget->setCurrentWidget(ui->groupChatWidget);  // ✅ 그룹채팅 위젯으로 전환
}
void MainWindow::showMainChat() {
    ui->stackedWidget->setCurrentWidget(ui->mainChatWidget);
}

void MainWindow::createNewRoom() {
    CreateRoomDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString roomName = dialog.getRoomName().trimmed();
        QString password = dialog.getPassword().trimmed();
        bool isPrivate = dialog.isPrivate();

        if (roomName.isEmpty()) {
            QMessageBox::warning(this, "경고", "방 이름을 입력하세요.");
            return;
        }

        QString command = QString("/create_room %1").arg(roomName);
        if (isPrivate && !password.isEmpty()) {
            command += QString(" --private --password %1").arg(password);
        }

        currentRoomName = roomName;
        chatClient->sendMessage(command.toStdString());

        // ✅ 새로 생성한 방 목록 요청
        chatClient->sendMessage("/list_rooms");

        // ✅ 새 창 열기
        if (!groupChatWindow) {
            groupChatWindow = std::make_unique<GroupChatWindow>(this);
            connect(groupChatWindow.get(), &GroupChatWindow::sendMessageRequested, this, &MainWindow::sendGroupMessage);
        }

        groupChatWindow->setRoomTitle(roomName);
        groupChatWindow->show();
    }
}

void MainWindow::joinSelectedRoom(QListWidgetItem* item) {
    if (!item) return;
    QString roomName = item->text();
    currentRoomName = roomName;

    chatClient->sendMessage(QString("/join_room %1").arg(roomName).toStdString());

    if (!groupChatWindow) {
        groupChatWindow = std::make_unique<GroupChatWindow>(this);
        connect(groupChatWindow.get(), &GroupChatWindow::sendMessageRequested, this, &MainWindow::sendGroupMessage);
    }

    groupChatWindow->setRoomTitle(roomName);
    groupChatWindow->show();
}

void MainWindow::sendGroupMessage(const QString& msg) {
    if (msg.isEmpty() || currentRoomName.isEmpty()) return;

    QString command = QString("/room_msg %1 %2").arg(currentRoomName).arg(msg);
    chatClient->sendMessage(command.toStdString());
}