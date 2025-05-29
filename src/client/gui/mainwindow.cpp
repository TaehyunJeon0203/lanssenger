#include "mainwindow.hpp"
#include "ui_mainwindow.h"  // CMake가 자동 생성한 ui 헤더

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::sendMessage);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::sendMessage() {
    QString message = ui->myChat->toPlainText().trimmed();
    if (!message.isEmpty()) {
        ui->currentChat->append(message);
        ui->myChat->clear();
    }
}
