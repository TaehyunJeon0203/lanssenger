#ifndef NICKNAME_WIDGET_HPP
#define NICKNAME_WIDGET_HPP

#include <QWidget>
#include <QString>

namespace Ui {
    class Form;
}

class NicknameWidget : public QWidget {
    Q_OBJECT

public:
    explicit NicknameWidget(QWidget *parent = nullptr);
    ~NicknameWidget();

    QString getNickname() const;

private slots:
    void onNicknameConfirm();

private:
    Ui::Form *ui;
    QString nickname_;
};

#endif // NICKNAME_WIDGET_HPP