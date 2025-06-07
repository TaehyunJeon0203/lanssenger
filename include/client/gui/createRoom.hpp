#pragma once

#include <QDialog>

namespace Ui {
class Dialog;
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
    Ui::Dialog *ui;
};
