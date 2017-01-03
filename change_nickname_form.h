#ifndef CHANGE_NICKNAME_FORM_H
#define CHANGE_NICKNAME_FORM_H

#include <QWidget>

namespace Ui {
class ChangeNicknameForm;
}

class ChangeNicknameForm : public QWidget
{
  Q_OBJECT

public:
  explicit ChangeNicknameForm(QWidget *parent = 0);
  ~ChangeNicknameForm();

private:
  Ui::ChangeNicknameForm *ui;
};

#endif // CHANGE_NICKNAME_FORM_H
