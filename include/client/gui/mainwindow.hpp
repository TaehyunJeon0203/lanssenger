#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <memory>
#include <QListWidgetItem>
#include <vector>
#include "client/gui/groupchatwindow.hpp"

class UserListWindow;
class ChatClient;
class GroupChatWindow; 

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<ChatClient> chatClient;
    std::unique_ptr<UserListWindow> userListWindow;
    std::vector<std::unique_ptr<GroupChatWindow>> groupChatWindows;
    QString currentRoomName;

    void setupMenuBar();
    void setupConnections();
    void connectToServer();

private slots:
    void sendMessage();
    void appendMessage(const QString &message);
    void requestUserList();

    // 🔽 새로 추가된 그룹채팅 관련 슬롯
    void showGroupChat();
    void showMainChat();
    void createNewRoom();
    void joinSelectedRoom();
    void joinSelectedPrivateRoom();
    void sendGroupMessage(const QString& msg);
    void requestRoomUserList(const QString& roomName);
};

#endif // MAINWINDOW_HPP
