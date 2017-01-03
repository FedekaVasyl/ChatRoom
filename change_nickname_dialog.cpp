#include "change_nickname_dialog.h"
#include "ui_change_nickname_dialog.h"

ChangeNicknameDialog::ChangeNicknameDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ChangeNicknameDialog)
{
  ui->setupUi(this);
  ui->change_edit->setPlaceholderText("Enter your new nickname");
  this->nickname = ui->change_edit;
  this->ok = ui->ok_button;
  this->cancel = ui->cancel_button;
}

ChangeNicknameDialog::~ChangeNicknameDialog()
{
  delete ui;
  delete nickname;
  delete ok;
  delete cancel;
}

QTextEdit* ChangeNicknameDialog::GetNickname() const
{
  return this->nickname;
}

QPushButton* ChangeNicknameDialog::GetOK() const
{
  return this->ok;
}

QPushButton* ChangeNicknameDialog::GetCancel() const
{
  return this->cancel;
}
