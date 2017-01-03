#include "message.h"
#include "error_message_form.h"
#ifndef ERROR_H
#define ERROR_H

#define NO_ERROR_MESSAGE 0


class Error : public Message
{
  Q_OBJECT
signals:
  void Show();
private:
  int error_code;
  ErrorMessageForm *error_form;
public slots:
  void ShowErrorForm();
public:
  Error();
  Error(int _code);   //Use only in main thread
  ~Error();
  int GetCode() const;
  void SetCode(int _code);
  ErrorMessageForm* GetErrorForm() const;
};

#endif // ERROR_H
