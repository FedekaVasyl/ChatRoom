#include "message.h"

Message::~Message()
{
  delete this->time;
}

string Message::GetMyMessage() const
{
  return this->message;
}

int Message::GetMessageLen() const
{
  return this->message_len;
}

void Message::SetMessage(string _message)
{
  this->message.clear();
  this->message = _message;
  this->message_len = this->message.size();
}

void Message::SetMessageLen(int _len)
{
  this->message_len = _len;
}

MyTime* Message::GetMyTime() const
{
  return this->time;
}

void Message::InsertTimeAndAuthor(string _author)
{
  string normal_form;
  normal_form.clear();
  normal_form = this->GetMyTime()->NormalForm();
  this->message += normal_form + _author + ":";
  this->message += kDelimiter;
  this->message_len = this->message.size();
}

void Message::AddToMessage(string _message)
{
  this->message += _message;
  this->message_len = this->message.size();
}

const string Message::GetLimiter() const
{
  return this->kLimiter;
}

void Message::ParseMessage(char *utility_letter, int &id)
{
  const int kBufLen = 2;
  char id_buf[kBufLen];
  std::size_t position = this->GetMyMessage().find(this->GetLimiter());
  memset(id_buf,0,kBufLen);
  memset(utility_letter,0,sizeof(utility_letter));
  strcpy(utility_letter,\
         this->GetMyMessage().substr(0,position).c_str());
  this->SetMessage(\
        this->GetMyMessage().substr(position+1,this->GetMessageLen()));
  position = this->GetMyMessage().find(this->GetLimiter());
  strcpy(id_buf,\
         this->GetMyMessage().substr(0,position).c_str());
  this->SetMessage(\
        this->GetMyMessage().substr(position+1,this->GetMessageLen()));
  string str = id_buf;
  id = std::stoi(str);
}
