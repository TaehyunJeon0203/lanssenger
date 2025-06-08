#include "client/gui/groupchatwindow.hpp"
#include "ui_groupchatwindow.h"
#include <QPoint>

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

    connect(ui->GCSendButton, &QPushButton::clicked, this, &GroupChatWindow::onSendButtonClicked);
    connect(ui->GCMessageInput, &QLineEdit::returnPressed, this, &GroupChatWindow::onSendButtonClicked);
}

GroupChatWindow::~GroupChatWindow() {
    delete ui;
}

void GroupChatWindow::appendMessage(const QString& msg) {
    ui->GCChatDisplay->append(msg);
}

QString GroupChatWindow::getInputText() const {
    return ui->GCMessageInput->text().trimmed();
}

void GroupChatWindow::setRoomTitle(const QString& roomName) {
    roomTitle = roomName;
    ui->currentRoomLabel->setText("방 이름: " + roomName);
}

void GroupChatWindow::onSendButtonClicked() {
    QString msg = getInputText();
    if (!msg.isEmpty()) {
        emit sendMessageRequested(msg);
        ui->GCMessageInput->clear();
    }
}
