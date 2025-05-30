#include "client/gui/mainwindow.hpp"
#include "ui_mainwindow.h"  // CMake가 자동 생성한 ui 헤더

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendMessage()
{
    QString message = ui->messageInput->toPlainText();
    if (!message.isEmpty()) {
        ui->chatDisplay->append("나: " + message);
        ui->messageInput->clear();
    }
}
