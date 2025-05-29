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

    void setupUI();
    void setupConnections();
}; 