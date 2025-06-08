#pragma once

#include <QWidget>

namespace Ui {
class GroupChatWindow;
}

class GroupChatWindow : public QWidget {
    Q_OBJECT

public:
    explicit GroupChatWindow(QWidget *parent = nullptr);
    ~GroupChatWindow();

    void appendMessage(const QString& msg);
    QString getInputText() const;
    void setRoomTitle(const QString& roomName);

signals:
    void sendMessageRequested(const QString& message);

private slots:
    void onSendButtonClicked();  // 슬롯 이름 이대로 유지

private:
    Ui::GroupChatWindow *ui;
};
