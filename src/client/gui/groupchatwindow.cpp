#include "client/gui/groupchatwindow.hpp"
#include "ui_groupchatwindow.h"
#include <QPoint>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

GroupChatWindow::GroupChatWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::GroupChatWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    ui->GCChatDisplay->setReadOnly(true);

    if (parent) {
        QPoint parentPos = parent->mapToGlobal(parent->rect().topRight());
        move(parentPos.x() + 10, parentPos.y());
    }

    // 이전 메시지 로드
    loadMessageHistory();

    connect(ui->GCSendButton, &QPushButton::clicked, this, &GroupChatWindow::onSendButtonClicked);
    connect(ui->GCMessageInput, &QLineEdit::returnPressed, this, &GroupChatWindow::onSendButtonClicked);
}

GroupChatWindow::~GroupChatWindow() {
    delete ui;
}

void GroupChatWindow::loadMessageHistory() {
    QString historyPath = QDir::homePath() + "/.lanssenger/history/";
    QDir().mkpath(historyPath);
    
    QFile file(historyPath + roomTitle + ".txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            ui->GCChatDisplay->append(in.readLine());
        }
        file.close();
    }
}

void GroupChatWindow::saveMessage(const QString& message) {
    QString historyPath = QDir::homePath() + "/.lanssenger/history/";
    QDir().mkpath(historyPath);
    
    QFile file(historyPath + roomTitle + ".txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << message << "\n";
        file.close();
    }
}

void GroupChatWindow::appendMessage(const QString& msg) {
    ui->GCChatDisplay->append(msg);
    saveMessage(msg);
}

QString GroupChatWindow::getInputText() const {
    return ui->GCMessageInput->text().trimmed();
}

void GroupChatWindow::setRoomTitle(const QString& roomName) {
    roomTitle = roomName;
    ui->currentRoomLabel->setText("방 이름: " + roomName);
    loadMessageHistory(); // 방 제목이 설정될 때 메시지 히스토리 로드
}

void GroupChatWindow::onSendButtonClicked() {
    QString msg = getInputText();
    if (!msg.isEmpty()) {
        emit sendMessageRequested(msg);
        ui->GCMessageInput->clear();
    }
}
