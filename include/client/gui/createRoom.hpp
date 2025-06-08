#ifndef CREATEROOM_HPP
#define CREATEROOM_HPP

#include <QDialog>
#include <QString>

namespace Ui {
class CreateRoomDialog;
}

class CreateRoomDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateRoomDialog(QWidget *parent = nullptr);
    ~CreateRoomDialog();

    QString getRoomName() const;
    QString getPassword() const;
    bool isPrivate() const;

private slots:
    void on_checkBox_stateChanged(int state);

private:
    Ui::CreateRoomDialog *ui;
};

#endif // CREATEROOM_HPP
