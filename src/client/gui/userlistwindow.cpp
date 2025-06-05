#include "client/gui/userlistwindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton> 

UserListWindow::UserListWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("유저 목록");
    resize(300, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 상단에 닫기 버튼 배치
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch(); // 오른쪽 정렬
    QPushButton *closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(24, 24);
    closeButton->setToolTip("닫기");
    topLayout->addWidget(closeButton);
    mainLayout->addLayout(topLayout);

    userListWidget = new QListWidget(this);
    mainLayout->addWidget(userListWidget);

    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

    // 윈도우 X 버튼도 사용 가능
    setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint);
}

void UserListWindow::updateUserList(const QStringList& users) {
    userListWidget->clear();
    userListWidget->addItems(users);
}