#ifndef CHANGE_NICKNAME_DIALOG_H
#define CHANGE_NICKNAME_DIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>

namespace Ui {
class ChangeNicknameDialog;
}

class ChangeNicknameDialog : public QDialog
{
  Q_OBJECT
private:
  Ui::ChangeNicknameDialog *ui;
  QTextEdit *nickname;
  QPushButton *ok;
  QPushButton *cancel;
public:
  explicit ChangeNicknameDialog(QWidget *parent = 0);
  ~ChangeNicknameDialog();
  QTextEdit* GetNickname() const;
  QPushButton* GetOK() const;
  QPushButton* GetCancel() const;
};

#endif // CHANGE_NICKNAME_DIALOG_H
