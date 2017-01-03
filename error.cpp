#include "message.h"
#include "error.h"

Error::Error()
{
  this->error_code = NO_ERROR_MESSAGE;
  this->error_form = new ErrorMessageForm();
  QObject::connect(this->GetErrorForm(),\
                   SIGNAL(Show()),this,SLOT(ShowErrorForm()),\
                   Qt::ConnectionType::BlockingQueuedConnection);
}

Error::Error(int _code)
{
  this->error_code = _code;
  this->error_form = new ErrorMessageForm();
  QObject::connect(this,\
                   SIGNAL(Show()),this,SLOT(ShowErrorForm()));
}

Error::~Error()
{
  delete this->error_form;
}

int Error::GetCode() const
{
  return this->error_code;
}

void Error::SetCode(int _code)
{
  this->error_code = _code;
}

ErrorMessageForm* Error::GetErrorForm() const
{
  return this->error_form;
}

void Error::ShowErrorForm()
{
  this->GetErrorForm()->GetMessageLabel()\
      ->setText(QString::fromStdString(this->GetMyMessage()));
  this->GetErrorForm()->GetTimeLabel()\
      ->setText(QString::fromStdString(this->GetMyTime()->NormalForm()));
  this->GetErrorForm()->GetErrorCodeLabel()\
      ->setText("Error code: " + QString::number(this->GetCode()));
  this->GetErrorForm()->GetMessageLabel()\
      ->setAlignment(Qt::AlignCenter);
  this->GetErrorForm()->GetErrorCodeLabel()\
      ->setAlignment(Qt::AlignLeft);
  this->GetErrorForm()->GetTimeLabel()\
      ->setAlignment(Qt::AlignRight);
  this->GetErrorForm()->setModal(true);
  this->GetErrorForm()->show();
  this->GetErrorForm()->exec();
}
