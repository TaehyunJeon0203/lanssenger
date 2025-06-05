#include "client/gui/userlistwindow.hpp"
#include "ui_userlistwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton> 

UserListWindow::UserListWindow(QWidget *parent) : QWidget(parent), ui(new Ui::UserListWindow) {
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint);
    
    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
}

UserListWindow::~UserListWindow() {
    delete ui;
}

void UserListWindow::updateUserList(const QStringList& users) {
    ui->userListWidget->clear();
    ui->userListWidget->addItems(users);
}