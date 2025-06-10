#ifndef USERLISTWINDOW_HPP
#define USERLISTWINDOW_HPP

#include <QWidget>
#include <memory>
#include <QStringList>

namespace Ui {
class UserListWindow;
}

class UserListWindow : public QWidget {
    Q_OBJECT

public:
    explicit UserListWindow(QWidget *parent = nullptr);
    ~UserListWindow();

    void updateUserList(const QStringList& users);
    void setTitle(const QString& title);

private:
    Ui::UserListWindow *ui;
};

#endif // USERLISTWINDOW_HPP