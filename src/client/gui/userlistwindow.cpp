#include "client/gui/userlistwindow.hpp"
#include <QVBoxLayout>
#include <QPushButton> 

UserListWindow::UserListWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("유저 목록");
    resize(300, 400);
    QVBoxLayout *layout = new QVBoxLayout(this);
    userListWidget = new QListWidget(this);
    layout->addWidget(userListWidget);

    // 닫기 버튼 제거 (우측 상단 X 버튼만 사용)
    setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint);
}

void UserListWindow::updateUserList(const QStringList& users) {
    userListWidget->clear();
    userListWidget->addItems(users);
}