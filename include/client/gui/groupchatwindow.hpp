#pragma once

#include <QWidget>
#include <QString>

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

signals:
    void sendMessageRequested(const QString& message);

private slots:
    void onSendButtonClicked();

private:
    void loadMessageHistory();
    void saveMessage(const QString& message);

    Ui::GroupChatWindow *ui;
    QString roomTitle;
};
