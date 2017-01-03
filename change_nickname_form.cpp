#include "change_nickname_form.h"
#include "ui_change_nickname_form.h"

ChangeNicknameForm::ChangeNicknameForm(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ChangeNicknameForm)
{
  ui->setupUi(this);
}

ChangeNicknameForm::~ChangeNicknameForm()
{
  delete ui;
}
