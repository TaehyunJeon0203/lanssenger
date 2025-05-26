#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <memory>

// ChatClient 클래스 전방 선언
class ChatClient;

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
    // UI 컴포넌트
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    QTextEdit* chatDisplay;
    QLineEdit* messageInput;
    QPushButton* sendButton;
    QListWidget* userList;
    QLabel* statusLabel;

    // 네트워크 클라이언트
    std::unique_ptr<ChatClient> chatClient;

    void setupUI();
    void setupConnections();
    void connectToServer();
}; 