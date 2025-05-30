#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui { class MainWindow; }  // Qt Designer 클래스 이름에 맞게 수정

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendMessage();

private:
    Ui::MainWindow *ui;  // Ui 클래스 이름 일치해야 함
};

#endif // MAINWINDOW_HPP
