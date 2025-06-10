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
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->chatDisplay->setReadOnly(true);
    setupMenuBar();
    setupConnections();
    connectToServer();

    // 초기 화면을 전체 채팅방으로 설정
    ui->stackedWidget->setCurrentWidget(ui->mainChatWidget);

    // 서버 연결 후 잠시 대기했다가 방 목록 요청
    QTimer::singleShot(1000, this, [this]() {
        if (chatClient) {
            chatClient->sendMessage("/list_rooms");
        }
    });
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
    connect(ui->privateRoomListWidget, &QListWidget::itemClicked, this, &MainWindow::joinSelectedPrivateRoom);
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
        if (message.startsWith("ROOM_LIST:")) {
            QStringList rooms = message.mid(10).split(",", Qt::SkipEmptyParts);
            ui->roomListWidget->clear();
            ui->privateRoomListWidget->clear();
            
            for (const QString& room : rooms) {
                if (room.startsWith("PUBLIC:")) {
                    QString roomName = room.mid(7); // "PUBLIC:" 제거
                    ui->roomListWidget->addItem(roomName);
                } else if (room.startsWith("PRIVATE:")) {
                    QString roomName = room.mid(8); // "PRIVATE:" 제거
                    ui->privateRoomListWidget->addItem(roomName);
                }
            }
        } else if (message.startsWith("USER_LIST:")) {
            QStringList users = message.mid(10).split(",", Qt::SkipEmptyParts);
            if (!userListWindow) {
                userListWindow = std::make_unique<UserListWindow>(this);
            }
            userListWindow->updateUserList(users);
            userListWindow->show();
            userListWindow->raise();
            userListWindow->activateWindow();
        } else if (message.startsWith("ROOM_MSG:")) {
            QString roomMsg = message.mid(9);
            for (const auto& window : groupChatWindows) {
                if (window->getRoomTitle() == currentRoomName) {
                    window->appendMessage(roomMsg);
                    break;
                }
            }
        } else if (message.startsWith("ROOM_USER_LIST:")) {
            QStringList users = message.mid(15).split(",", Qt::SkipEmptyParts);
            
            // 현재 열려 있는 그룹채팅 창 중 roomTitle이 같은 창 찾아서 updateUserList 호출
            for (const auto& window : groupChatWindows) {
                if (window->getRoomTitle() == currentRoomName) {
                    window->updateUserList(users);
                    break;
                }
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

        // 방 생성 후 잠시 대기했다가 방 목록 새로고침
        QTimer::singleShot(500, this, [this]() {
            chatClient->sendMessage("/list_rooms");
        });

        // 새 창 열기
        auto newGroupChatWindow = std::make_unique<GroupChatWindow>(this);
        connect(newGroupChatWindow.get(), &GroupChatWindow::sendMessageRequested, this, &MainWindow::sendGroupMessage);
        connect(newGroupChatWindow.get(), &GroupChatWindow::requestRoomUserList, this, &MainWindow::requestRoomUserList);
        newGroupChatWindow->setRoomTitle(roomName);
        newGroupChatWindow->show();
        groupChatWindows.push_back(std::move(newGroupChatWindow));
    }
}

void MainWindow::joinSelectedRoom() {
    QListWidgetItem* selectedItem = ui->roomListWidget->currentItem();
    if (selectedItem) {
        QString roomName = selectedItem->text();
        currentRoomName = roomName;
        chatClient->sendMessage(QString("/join_room %1").arg(roomName).toStdString());

        // 이미 열려있는 창이 있는지 확인
        for (const auto& window : groupChatWindows) {
            if (window->getRoomTitle() == roomName) {
                window->show();
                window->raise();
                window->activateWindow();
                return;
            }
        }

        // 열려있는 창이 없으면 새로 생성
        auto newGroupChatWindow = std::make_unique<GroupChatWindow>(this);
        connect(newGroupChatWindow.get(), &GroupChatWindow::sendMessageRequested, this, &MainWindow::sendGroupMessage);
        connect(newGroupChatWindow.get(), &GroupChatWindow::requestRoomUserList, this, &MainWindow::requestRoomUserList);
        newGroupChatWindow->setRoomTitle(roomName);
        newGroupChatWindow->show();
        groupChatWindows.push_back(std::move(newGroupChatWindow));
    }
}

void MainWindow::joinSelectedPrivateRoom() {
    QListWidgetItem* selectedItem = ui->privateRoomListWidget->currentItem();
    if (selectedItem) {
        QString roomName = selectedItem->text();
        bool ok;
        QString password = QInputDialog::getText(this, "비밀번호 입력",
            "비공개 방 비밀번호를 입력하세요:", QLineEdit::Password, "", &ok);
        
        if (ok && !password.isEmpty()) {
            currentRoomName = roomName;
            QString command = QString("/join_room %1 --password %2").arg(roomName).arg(password);
            chatClient->sendMessage(command.toStdString());

            // 이미 열려있는 창이 있는지 확인
            for (const auto& window : groupChatWindows) {
                if (window->getRoomTitle() == roomName) {
                    window->show();
                    window->raise();
                    window->activateWindow();
                    return;
                }
            }

            // 열려있는 창이 없으면 새로 생성
            auto newGroupChatWindow = std::make_unique<GroupChatWindow>(this);
            connect(newGroupChatWindow.get(), &GroupChatWindow::sendMessageRequested, this, &MainWindow::sendGroupMessage);
            connect(newGroupChatWindow.get(), &GroupChatWindow::requestRoomUserList, this, &MainWindow::requestRoomUserList);
            newGroupChatWindow->setRoomTitle(roomName);
            newGroupChatWindow->show();
            groupChatWindows.push_back(std::move(newGroupChatWindow));
        }
    }
}

void MainWindow::sendGroupMessage(const QString& msg) {
    if (msg.isEmpty() || currentRoomName.isEmpty()) return;

    QString command = QString("/room_msg %1 %2").arg(currentRoomName).arg(msg);
    chatClient->sendMessage(command.toStdString());
}

void MainWindow::requestRoomUserList(const QString& roomName) {
    if (chatClient) {
        chatClient->sendMessage(QString("/room_users %1").arg(roomName).toStdString());
    }
}