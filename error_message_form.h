#ifndef ERROR_MESSAGE_FORM_H
#define ERROR_MESSAGE_FORM_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>

namespace Ui {
class ErrorMessageForm;
}

class ErrorMessageForm : public QDialog
{
  Q_OBJECT
private:
  Ui::ErrorMessageForm *ui;
  QLabel *time_label;
  QLabel *message_label;
  QLabel *error_code_label;
  QPushButton *ok_button;
public:
  explicit ErrorMessageForm(QWidget *parent = 0);
  ~ErrorMessageForm();
  QLabel* GetTimeLabel() const;
  QLabel* GetMessageLabel() const;
  QLabel* GetErrorCodeLabel() const;
  QPushButton* GetOKButton() const;
};

#endif // ERROR_MESSAGE_FORM_H
