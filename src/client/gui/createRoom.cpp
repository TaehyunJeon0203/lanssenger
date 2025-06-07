#include "client/gui/createRoom.hpp"
#include "ui_createRoom.h"

CreateRoomDialog::CreateRoomDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->lineEdit_2->setEnabled(false);

    connect(ui->checkBox, &QCheckBox::stateChanged,
            this, &CreateRoomDialog::on_checkBox_stateChanged);
}

CreateRoomDialog::~CreateRoomDialog() {
    delete ui;
}

void CreateRoomDialog::on_checkBox_stateChanged(int state) {
    ui->lineEdit_2->setEnabled(state == Qt::Checked);
}

QString CreateRoomDialog::getRoomName() const {
    return ui->lineEdit->text();
}

QString CreateRoomDialog::getPassword() const {
    return ui->lineEdit_2->text();
}

bool CreateRoomDialog::isPrivate() const {
    return ui->checkBox->isChecked();
}
