#include "client/gui/groupchatwindow.hpp"
#include "ui_groupchatwindow.h"
#include "client/gui/userlistwindow.hpp"
#include <QPoint>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QCloseEvent>
#include "client/gui/mainwindow.hpp"

GroupChatWindow::GroupChatWindow(const QString& roomName, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::GroupChatWindow)
    , roomName_(roomName)
{
    ui->setupUi(this);
    setWindowTitle(roomName);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    ui->GCChatDisplay->setReadOnly(true);

    if (parent) {
        QPoint parentPos = parent->mapToGlobal(parent->rect().topRight());
        move(parentPos.x() + 10, parentPos.y());
    }

    // 이전 메시지 로드
    loadMessageHistory();

    // 방 입장 요청
    QString joinCommand = QString("/join_room %1\n").arg(roomName);
    emit sendCommand(joinCommand);

    // 사용자 목록 요청
    QString userListCommand = QString("/room_users %1\n").arg(roomName);
    emit sendCommand(userListCommand);

    // 버튼 연결
    connect(ui->GCSendButton, &QPushButton::clicked, this, &GroupChatWindow::onSendButtonClicked);
    connect(ui->GCMessageInput, &QLineEdit::returnPressed, this, &GroupChatWindow::onSendButtonClicked);
    connect(ui->GCUserListButton, &QPushButton::clicked, this, &GroupChatWindow::onUserListButtonClicked);
}

GroupChatWindow::~GroupChatWindow() {
    // 방 퇴장 요청
    QString leaveCommand = QString("/leave_room %1\n").arg(roomName_);
    emit sendCommand(leaveCommand);
    delete ui;
}

void GroupChatWindow::loadMessageHistory() {
    QString historyPath = QDir::homePath() + "/.lanssenger/history/";
    QDir().mkpath(historyPath);
    
    QFile file(historyPath + roomName_ + ".txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            ui->GCChatDisplay->append(in.readLine());
        }
        file.close();
    }
}

void GroupChatWindow::saveMessageHistory(const QString& message) {
    QString historyPath = QDir::homePath() + "/.lanssenger/history/";
    QDir().mkpath(historyPath);
    
    QFile file(historyPath + roomName_ + ".txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << message << "\n";
        file.close();
    }
}

void GroupChatWindow::appendMessage(const QString& msg) {
    // 메시지에서 방 이름 부분 제거 (예: "채팅방 [방이름] 닉네임(ip): 메시지" -> "닉네임(ip): 메시지")
    QString displayMsg = msg;
    int start = displayMsg.indexOf("]") + 2;  // "] " 다음부터
    if (start > 1) {
        displayMsg = displayMsg.mid(start);
    }
    ui->GCChatDisplay->append(displayMsg);
    saveMessageHistory(displayMsg);
}

QString GroupChatWindow::getInputText() const {
    return ui->GCMessageInput->text().trimmed();
}

void GroupChatWindow::setRoomTitle(const QString& roomName) {
    roomName_ = roomName;
    ui->currentRoomLabel->setText("방 이름: " + roomName);
    loadMessageHistory(); // 방 제목이 설정될 때 메시지 히스토리 로드
}

void GroupChatWindow::onSendButtonClicked() {
    QString msg = getInputText();
    if (!msg.isEmpty()) {
        QString command = QString("/room_msg %1 %2\n").arg(roomName_).arg(msg);
        emit sendCommand(command);
        ui->GCMessageInput->clear();
    }
}

void GroupChatWindow::onUserListButtonClicked() {
    if (!roomName_.isEmpty()) {
        emit requestRoomUserList(roomName_);  // MainWindow에서 /room_users 요청

        if (!userListWindow) {
            userListWindow = new UserListWindow(this);
            userListWindow->setTitle("대화 상대");
        }

        // 사용자 목록 창을 먼저 표시
        userListWindow->show();
        userListWindow->raise();
        userListWindow->activateWindow();
    }
}

void GroupChatWindow::updateUserList(const QStringList& users) {
    if (!userListWindow) {
        userListWindow = new UserListWindow(this);
        userListWindow->setTitle("대화 상대");
    }
    userListWindow->updateUserList(users);
    userListWindow->show();
    userListWindow->raise();
    userListWindow->activateWindow();
}

void GroupChatWindow::closeEvent(QCloseEvent* event) {
    // 방 퇴장 요청
    QString leaveCommand = QString("/leave_room %1\n").arg(roomName_);
    emit sendCommand(leaveCommand);
    event->accept();
}