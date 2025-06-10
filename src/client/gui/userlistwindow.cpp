#include "client/gui/userlistwindow.hpp"
#include "ui_userlistwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton> 

UserListWindow::UserListWindow(QWidget *parent) : QWidget(parent), ui(new Ui::UserListWindow) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    
    // 메인 윈도우가 있다면 그 위치를 기준으로 설정
    if (parent) {
        QPoint parentPos = parent->mapToGlobal(parent->rect().topRight());
        move(parentPos.x() + 10, parentPos.y()); // 메인 윈도우 오른쪽에 10픽셀 간격으로 배치
    }
    
    // 기본 제목 설정
    setTitle("유저 목록");
}

UserListWindow::~UserListWindow() {
    delete ui;
}

void UserListWindow::updateUserList(const QStringList& users) {
    ui->userListWidget->clear();
    ui->userListWidget->addItems(users);
}

void UserListWindow::setTitle(const QString& title) {
    setWindowTitle(title);
}