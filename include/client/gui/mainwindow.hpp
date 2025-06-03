#pragma once

#include <QMainWindow>
#include <memory>
#include <string>
#include "userlistwindow.hpp"

// ChatClient 클래스 전방 선언
class ChatClient;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendMessage();
    void appendMessage(const QString& message);
    //void showGroupChat(); 그룹채팅 구현시 사용
    //void showMainChat();
    void requestUserList();

private:
    void setupConnections();
    void connectToServer();

    Ui::MainWindow *ui;
    std::unique_ptr<ChatClient> chatClient;
    std::unique_ptr<UserListWindow> userListWindow;
};
