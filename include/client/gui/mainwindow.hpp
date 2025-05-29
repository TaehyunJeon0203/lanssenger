#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }  // Qt Designer 클래스 이름에 맞게 수정
QT_END_NAMESPACE

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
