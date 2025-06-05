#include "client/gui/userlistwindow.hpp"
#include <QVBoxLayout>

UserListWindow::UserListWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("유저 목록");
    resize(300, 400);
    QVBoxLayout *layout = new QVBoxLayout(this);
    userListWidget = new QListWidget(this);
    layout->addWidget(userListWidget);
}

void UserListWindow::updateUserList(const QStringList& users) {
    userListWidget->clear();
    userListWidget->addItems(users);
}#include "client/gui/userlistwindow.hpp"
#include <QVBoxLayout>
#include <QPushButton> 

UserListWindow::UserListWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("유저 목록");
    resize(300, 400);
    QVBoxLayout *layout = new QVBoxLayout(this);
    userListWidget = new QListWidget(this);
    layout->addWidget(userListWidget);


    QPushButton *closeButton = new QPushButton("닫기", this);
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
}

void UserListWindow::updateUserList(const QStringList& users) {
    userListWidget->clear();
    userListWidget->addItems(users);
}