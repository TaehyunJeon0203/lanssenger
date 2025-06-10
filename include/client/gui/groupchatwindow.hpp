#pragma once

#include <QWidget>
#include <QCloseEvent>
#include <QString>
#include <memory>

class UserListWindow;

namespace Ui {
class GroupChatWindow;
}

class GroupChatWindow : public QWidget {
    Q_OBJECT

public:
    explicit GroupChatWindow(const QString& roomName, QWidget *parent = nullptr);
    ~GroupChatWindow();

    void appendMessage(const QString& msg);
    void setRoomTitle(const QString& roomName);
    void updateUserList(const QStringList& users);
    QString getInputText() const;
    QString getRoomTitle() const { return roomName_; }

protected:
    void closeEvent(QCloseEvent* event) override;

signals:
    void sendCommand(const QString& command);
    void requestRoomUserList(const QString& roomName);

private slots:
    void onSendButtonClicked();
    void onUserListButtonClicked();

private:
    void loadMessageHistory();
    void saveMessageHistory(const QString& message);

    Ui::GroupChatWindow *ui;
    QString roomName_;
    UserListWindow* userListWindow = nullptr;
};
