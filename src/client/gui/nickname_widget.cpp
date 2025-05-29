#include "nickname_widget.hpp"
#include "ui_nicknameSet.h" // 이건 Qt가 생성한 UI 클래스
#include "client/nickname_manager.hpp"

#include <QMessageBox>

NicknameWidget::NicknameWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked,
            this, &NicknameWidget::onNicknameConfirm);
}

NicknameWidget::~NicknameWidget() {
    delete ui;
}

void NicknameWidget::onNicknameConfirm() {
    QString input = ui->textEdit_2->toPlainText().trimmed();

    NicknameManager manager;
    std::string nickname = manager.generateNickname(input.toStdString());

    if (!manager.isValidNickname(nickname)) {
        QMessageBox::warning(this, "오류", "닉네임 형식이 올바르지 않습니다.");
        return;
    }

    if (!manager.isNicknameAvailable(nickname)) {
        QMessageBox::warning(this, "중복", "이미 사용 중인 닉네임입니다.");
        return;
    }

    manager.registerNickname(nickname);
    nickname_ = QString::fromStdString(nickname);
    QMessageBox::information(this, "등록 완료", "닉네임이 설정되었습니다: " + nickname_);
}

QString NicknameWidget::getNickname() const {
    return nickname_;
}
