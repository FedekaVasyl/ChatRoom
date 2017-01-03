#include "chat_form.h"
#include "ui_chat_form.h"

ChatForm::ChatForm(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ChatForm)
{
  ui->setupUi(this);
  this->change_edit = ui->nickname_edit;
  this->message_edit = ui->message_edit;
  this->leave_chat = ui->leave_chat_button;
  this->change_nickname = ui->change_button;
  this->log_in = ui->log_in_button;
  this->say = ui->say_button;
  this->chat_body = ui->chat_body;
  this->client_list = ui->client_list;
  this->chat_name_lable = ui->chat_name_lable;
  this->list_model = new QStringListModel(this);
  QStringList data_list;
  this->list_model->setStringList(data_list);
  ui->client_list->setModel(this->list_model);
  ui->client_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->nickname_edit->setPlaceholderText("Enter Your nickname");
  ui->message_edit->setPlaceholderText("Enter your message");
}

ChatForm::~ChatForm()
{
  delete ui;
  delete change_edit;
  delete message_edit;
  delete leave_chat;
  delete change_nickname;
  delete log_in;
  delete say;
  delete chat_body;
  delete client_list;
  delete list_model;
  delete chat_name_lable;
}

QTextEdit* ChatForm::GetChangeEdit() const
{
  return this->change_edit;
}

QTextEdit* ChatForm::GetMessageEdit() const
{
  return this->message_edit;
}

QPushButton* ChatForm::GetLeaveChat() const
{
  return this->leave_chat;
}

QPushButton* ChatForm::GetChangeNickname() const
{
  return this->change_nickname;
}

QPushButton* ChatForm::GetLogIn() const
{
  return this->log_in;
}

QPushButton* ChatForm::GetSay() const
{
  return this->say;
}

QTextBrowser* ChatForm::GetChatBody() const
{
  return this->chat_body;
}

QListView* ChatForm::GetClientList() const
{
  return this->client_list;
}

QStringListModel* ChatForm::GetListModel() const
{
  return this->list_model;
}

QLabel* ChatForm::GetChatNameLable() const
{
  return this->chat_name_lable;
}

void ChatForm::SetIntoClientList(string _client)
{
  this->list_model->insertRow(this->list_model->rowCount());
  QModelIndex index = this->list_model->index(this->list_model->rowCount() - 1);
  this->list_model->setData(index,QString::fromStdString(_client));
  this->client_list->setUpdatesEnabled(true);
}

void ChatForm::DeleteFromClientList(string _client)
{
  this->client_list->setUpdatesEnabled(false);
  QModelIndexList index_list = this->list_model->match\
      (this->list_model->index(0,0), Qt::DisplayRole, \
       QString::fromStdString(_client), -1, Qt::MatchStartsWith);
  QModelIndex index = index_list.first();
  this->client_list->setUpdatesEnabled(true);
}

void ChatForm::Show()
{
  this->GetChangeEdit()->setEnabled(true);
  this->GetChangeEdit()->clear();
  this->GetChangeEdit()->setPlaceholderText("Enter Your nickname");
  this->GetChangeNickname()->setEnabled(true);
  //this->GetChatBody()->clear();
  this->GetChatBody()->setEnabled(true);
  this->GetLeaveChat()->setEnabled(true);
  this->GetLogIn()->setEnabled(true);
  this->GetMessageEdit()->setEnabled(true);
  this->GetMessageEdit()->clear();
  this->GetMessageEdit()->setPlaceholderText("Enter your message");
  this->GetSay()->setEnabled(true);
  QAbstractItemModel* model = this->GetClientList()->model();
  this->GetClientList()->setUpdatesEnabled(false);
  for(int i = 0; i < model->rowCount(); i++) {
    model->removeRow(i);
  }
  this->GetClientList()->setUpdatesEnabled(true);
  this->show();
}

void ChatForm::UpdaiteClientList(string _str)
{
  Message *message = new Message(_str);
  int i = 0;
  QAbstractItemModel* model = this->GetClientList()->model();

  this->GetClientList()->setUpdatesEnabled(false);
  for(int i = 0; i < model->rowCount(); i++) {
      model->removeRow(i);
  }
  this->GetClientList()->setUpdatesEnabled(true);
  while(!message->GetMyMessage().empty()) {
    string str;
    str.clear();
    std::size_t position = message->GetMyMessage().find(message->GetLimiter());
    str = message->GetMyMessage().substr(0,position);
    message->SetMessage(\
          message->GetMyMessage().substr(position+1,message->GetMessageLen()));
    if(str.empty()) {
      break;
    }
    this->SetIntoClientList(str);
    /*if(i == 0) {
      this->GetChangeEdit()->\
          setText(QString::fromStdString(str));
    }
    i++;*/
  }
  delete message;
}
