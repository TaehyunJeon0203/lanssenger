#pragma once

#include <QMainWindow>
#include <memory>
#include <string>

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
    void updateUserList(const QStringList& users);

private:
    void setupConnections();
    void connectToServer();

    Ui::MainWindow *ui;
    std::unique_ptr<ChatClient> chatClient;
}; 