#pragma once

#include <QWidget>
#include <QString>
#include <memory>

class UserListWindow;

namespace Ui {
class GroupChatWindow;
}

class GroupChatWindow : public QWidget {
    Q_OBJECT

public:
    explicit GroupChatWindow(QWidget *parent = nullptr);
    ~GroupChatWindow();

    void setRoomTitle(const QString& roomName);
    void appendMessage(const QString& msg);
    QString getInputText() const;
    QString getRoomTitle() const { return roomTitle; }
    void updateUserList(const QStringList& users);

signals:
    void sendMessageRequested(const QString& message);
    void requestRoomUserList(const QString& roomName);

private slots:
    void onSendButtonClicked();
    void onUserListButtonClicked();

private:
    void loadMessageHistory();
    void saveMessage(const QString& message);

    Ui::GroupChatWindow *ui;
    QString roomTitle;

    std::unique_ptr<UserListWindow> userListWindow;
};
