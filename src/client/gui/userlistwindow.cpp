#include "client/gui/userlistwindow.hpp"
#include "ui_userlistwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton> 
#include <QShowEvent>

UserListWindow::UserListWindow(QWidget *parent) : QWidget(parent), ui(new Ui::UserListWindow) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    
    // 기본 제목 설정
    setTitle("유저 목록");
}

UserListWindow::~UserListWindow() {
    delete ui;
}

    // 메인 윈도우가 있다면 그 위치를 기준으로 설정
void UserListWindow::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (parentWidget()) {
        QPoint parentPos = parentWidget()->mapToGlobal(parentWidget()->rect().topLeft());
        move(parentPos.x() - width() - 10, parentPos.y());
    }
}

void UserListWindow::updateUserList(const QStringList& users) {
    ui->userListWidget->clear();
    for (const QString& user : users) {
        if (!user.isEmpty()) {
            ui->userListWidget->addItem(user);
        }
    }
}

void UserListWindow::setTitle(const QString& title) {
    setWindowTitle(title);
}