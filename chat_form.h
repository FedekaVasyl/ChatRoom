#ifndef CHAT_FORM_H
#define CHAT_FORM_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QListView>
#include <QStringListModel>
#include <QStringList>
#include <QLabel>
#include "message.h"

using std::string;

namespace Ui {
class ChatForm;
}

class ChatForm : public QWidget
{
  Q_OBJECT
private slots:
  void Show();
private:
  Ui::ChatForm *ui;
  QTextEdit *change_edit;
  QTextEdit *message_edit;
  QPushButton *leave_chat;
  QPushButton *change_nickname;
  QPushButton *log_in;
  QPushButton *say;
  QTextBrowser *chat_body;
  QListView *client_list;
  QStringListModel *list_model;
  QLabel *chat_name_lable;
public:
  explicit ChatForm(QWidget *parent = 0);
  ~ChatForm();
  QTextEdit* GetChangeEdit() const;
  QTextEdit* GetMessageEdit() const;
  QPushButton* GetLeaveChat() const;
  QPushButton* GetChangeNickname() const;
  QPushButton* GetLogIn() const;
  QPushButton* GetSay() const;
  QTextBrowser* GetChatBody() const;
  QListView *GetClientList() const;
  QStringListModel *GetListModel() const;
  QLabel *GetChatNameLable() const;
  void SetIntoClientList(string _client);
  void DeleteFromClientList(string _client);
  void UpdaiteClientList(string _str);
};

#endif // CHAT_FORM_H
