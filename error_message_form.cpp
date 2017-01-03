#include "error_message_form.h"
#include "ui_error_message_form.h"

ErrorMessageForm::ErrorMessageForm(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ErrorMessageForm)
{
  ui->setupUi(this);
  this->time_label = ui->time_label;
  this->message_label = ui->message_label;
  this->error_code_label = ui->code_error_label;
  this->ok_button = ui->ok_button;
  QObject::connect(this->ok_button,\
                   SIGNAL(clicked(bool)),this,SLOT(reject()));
}

ErrorMessageForm::~ErrorMessageForm()
{
  delete ui;
  delete time_label;
  delete message_label;
  delete ok_button;
}

QLabel* ErrorMessageForm::GetTimeLabel() const
{
  return this->time_label;
}

QLabel* ErrorMessageForm::GetMessageLabel() const
{
  return this->message_label;
}

QLabel* ErrorMessageForm::GetErrorCodeLabel() const
{
  return this->error_code_label;
}

QPushButton* ErrorMessageForm::GetOKButton() const
{
  return this->ok_button;
}
