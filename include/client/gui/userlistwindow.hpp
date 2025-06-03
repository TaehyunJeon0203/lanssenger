#pragma once
#include <QWidget>
#include <QListWidget>

class UserListWindow : public QWidget {
    Q_OBJECT
public:
    explicit UserListWindow(QWidget *parent = nullptr);
    void updateUserList(const QStringList& users);
private:
    QListWidget *userListWidget;
};