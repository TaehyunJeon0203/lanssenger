#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <memory>

class UserListWindow;
class ChatClient;

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
};

#endif // MAINWINDOW_HPP
