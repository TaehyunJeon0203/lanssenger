#pragma once

#include <QMainWindow>
#include <memory>
#include "client/gui/userlistwindow.hpp"

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
    void requestUserList();

private:
    void setupConnections();
    void connectToServer();
    void appendMessage(const QString& message);

    Ui::MainWindow *ui;
    std::unique_ptr<class ChatClient> chatClient;
    std::unique_ptr<UserListWindow> userListWindow;
};
