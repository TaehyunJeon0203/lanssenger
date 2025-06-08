#include "client/gui/createRoom.hpp"
#include "ui_createRoom.h"

CreateRoomDialog::CreateRoomDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->setRoomCode->setEnabled(false);

    connect(ui->setCodeCheckBox, &QCheckBox::checkStateChanged,
            this, &CreateRoomDialog::on_checkBox_stateChanged);
}

CreateRoomDialog::~CreateRoomDialog() {
    delete ui;
}

void CreateRoomDialog::on_checkBox_stateChanged(int state) {
    ui->setRoomCode->setEnabled(state == Qt::Checked);
}

QString CreateRoomDialog::getRoomName() const {
    return ui->setRoomName->text();
}

QString CreateRoomDialog::getPassword() const {
    return ui->setRoomCode->text();
}

bool CreateRoomDialog::isPrivate() const {
    return ui->setCodeCheckBox->isChecked();
}
