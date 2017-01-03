#include "chat.h"

Chat::Chat()
{
  this->chat_name.clear();
  this->client_count = 0;
  this->chat_form = new ChatForm();
  this->change_dialog = new ChangeNicknameDialog();
  this->clients = new Client*[MAX_CLIENT_COUNT];
  this->current_client = new Client();
  this->receiving_handle = 0;
  for(int i = 0; i < MAX_CLIENT_COUNT; i++) {
    this->clients[i] = new Client();
  }
  this->chat_form->GetChangeNickname()->setEnabled(false);
  this->chat_form->GetChatBody()->setEnabled(false);
  this->chat_form->GetMessageEdit()->setEnabled(false);
  this->chat_form->GetSay()->setEnabled(false);
  QObject::connect(this,\
                   SIGNAL(ShowChatForm()),chat_form,SLOT(Show()));
  QObject::connect(this->GetChatForm()->GetLogIn(),\
                   SIGNAL(clicked(bool)),this,SLOT(LogInToChat()));
  QObject::connect(this->GetChatForm()->GetChangeNickname(),\
                   SIGNAL(clicked(bool)),this,SLOT(ChangeNicknameClick()));
  QObject::connect(this->GetDialog()->GetOK(),\
                   SIGNAL(clicked(bool)),this,SLOT(ChangeNickname()));
  QObject::connect(this->GetDialog()->GetCancel(),\
                   SIGNAL(clicked(bool)),this->GetDialog(),SLOT(reject()));
  QObject::connect(this->GetDialog()->GetOK(),\
                   SIGNAL(clicked(bool)),this->GetDialog(),SLOT(reject()));
  QObject::connect(this->GetChatForm()->GetSay(),\
                   SIGNAL(clicked(bool)),this,SLOT(SayClient()));
  QObject::connect(this->GetChatForm()->GetLeaveChat(),\
                   SIGNAL(clicked(bool)),this,SLOT(Disconnect()));
  QObject::connect(this->GetChatForm()->GetChangeEdit(),\
                   SIGNAL(textChanged()),this,SLOT(TextChanged()));
  QObject::connect(this->GetDialog()->GetNickname(),\
                   SIGNAL(textChanged()),this,SLOT(TextNicknameChanged()));
  QObject::connect(this->GetChatForm()->GetMessageEdit(),\
                   SIGNAL(textChanged()),this,SLOT(TextChanged()));

  this->StartFinding();

}

Chat::Chat(string _chat_name)
{
  this->client_count = 0;
  this->chat_name.clear();
  this->chat_name = _chat_name;
  this->chat_form = new ChatForm();
  this->change_dialog = new ChangeNicknameDialog();
  this->clients = new Client*[MAX_CLIENT_COUNT];
  this->current_client = new Client();
  this->receiving_handle = 0;
  for(int i = 0; i < MAX_CLIENT_COUNT; i++) {
    this->clients[i] = new Client();
  }
  this->chat_form->GetChangeNickname()->setEnabled(false);
  this->chat_form->GetChatBody()->setEnabled(false);
  this->chat_form->GetMessageEdit()->setEnabled(false);
  this->chat_form->GetSay()->setEnabled(false);
  QObject::connect(this,\
                   SIGNAL(ShowChatForm()),chat_form,SLOT(Show()));
  QObject::connect(this->GetChatForm()->GetLogIn(),\
                   SIGNAL(clicked(bool)),this,SLOT(LogInToChat()));
  QObject::connect(this->GetChatForm()->GetChangeNickname(),\
                   SIGNAL(clicked(bool)),this,SLOT(ChangeNicknameClick()));
  QObject::connect(this->GetDialog()->GetOK(),\
                   SIGNAL(clicked(bool)),this,SLOT(ChangeNickname()));
  QObject::connect(this->GetDialog()->GetCancel(),\
                   SIGNAL(clicked(bool)),this->GetDialog(),SLOT(reject()));
  QObject::connect(this->GetDialog()->GetOK(),\
                   SIGNAL(clicked(bool)),this->GetDialog(),SLOT(reject()));
  QObject::connect(this->GetChatForm()->GetSay(),\
                   SIGNAL(clicked(bool)),this,SLOT(SayClient()));
  QObject::connect(this->GetChatForm()->GetLeaveChat(),\
                   SIGNAL(clicked(bool)),this,SLOT(Disconnect()));
  QObject::connect(this->GetChatForm()->GetChangeEdit(),\
                   SIGNAL(textChanged()),this,SLOT(TextChanged()));
  QObject::connect(this->GetDialog()->GetNickname(),\
                   SIGNAL(textChanged()),this,SLOT(TextNicknameChanged()));
  QObject::connect(this->GetChatForm()->GetMessageEdit(),\
                   SIGNAL(textChanged()),this,SLOT(TextChanged()));
  this->StartFinding();
}

Chat::~Chat()
{
#ifdef _WIN32
  TerminateThread(this->receiving_handle, NULL);
  WaitForSingleObject(this->receiving_handle,INFINITE);
  CloseHandle(this->receiving_handle);
#else
  pthread_cancel(this->receiving_handle);
  pthread_join(this->receiving_handle,NULL);
#endif
  delete this->chat_form;
  delete this->change_dialog;
  delete this->current_client;
  for(int i = 0; i < MAX_CLIENT_COUNT; i++) {
    delete this->clients[i];
  }
  delete [] this->clients;
}

int Chat::GetClientCount() const
{
  return this->client_count;
}

string Chat::GetChatName() const
{
  return this->chat_name;
}

void Chat::SetChatName(string _chat_name)
{
  this->chat_name = _chat_name;
}

void Chat::SetClientCount(int _count)
{
  this->client_count = _count;
}

void Chat::SetClientNickname(string _nickname, int _position)
{
  this->clients[_position]->SetNickname(_nickname);
}

int Chat::FindNickname(string _nickname)
{
  for(int i = 0; i < MAX_CLIENT_COUNT; i++) {
    if(this->clients[i]->GetNickname() == _nickname) {
      return i;
    }
  }
  return NO_CLIENT;
}
#ifdef _WIN32
  HANDLE Chat::GetReceivingHandle() const
  {
    return this->receiving_handle;
  }

  void Chat::SetReceivingHandle(HANDLE _handle)
  {
    this->receiving_handle = _handle;
  }
#else
  pthread_t Chat::GetReceivingHandle() const
  {
    return this->receiving_handle;
  }

  void Chat::SetReceivingHandle(pthread_t _handle)
  {
    this->receiving_handle = _handle;
  }
#endif

ChatForm* Chat::GetChatForm() const
{
  return this->chat_form;
}

void Chat::StartFinding()
{
  this->GetTimer()->start();
  //this->timer->start();
#ifdef _WIN32
  this->SetFindingHandle((HANDLE) CreateThread(0,0,\
       (LPTHREAD_START_ROUTINE)this->FindingThreadFunction,\
                                               LPVOID(this),0,0));
#else
  int err = 0;
  pthread_t finding_thread;
  err = pthread_create(&finding_thread,NULL,&Chat::FindingThreadFunction,(void*)this);
          /*if (err != 0) {
            printf("\ncan't create thread :[%s]\n", strerror(err));
          }*/
  this->SetFindingHandle(finding_thread);
#endif
}
void Chat::TextChanged()
{
  string str = this->GetChatForm()->GetChangeEdit()->\
      toPlainText().toStdString();
  if(!str.empty()) {
    char key[2];
    strcpy(key,str.substr(str.size() - 1, str.size()).c_str());
    if(*key == '\n' && str.size() > 1) {
      emit this->GetChatForm()->GetChangeNickname()->click();
    }
  }
  if(str.size() > MAX_NICKNAME_LEN) {
    this->GetChatForm()->GetChangeEdit()->\
        setPlainText(QString::fromStdString\
                     (str.substr(0,str.size() - 1)));
    this->GetChatForm()->GetChangeNickname()->setEnabled(true);
    return;
  }
  if(!str.empty()) {
    this->GetChatForm()->GetChangeNickname()->setEnabled(true);
  }
  else {
    this->GetChatForm()->GetChangeNickname()->setEnabled(true);
  }
}

void Chat::TextNicknameChanged()
{
  string str = this->GetDialog()->GetNickname()->\
      toPlainText().toStdString();
  if(!str.empty()) {
    char key[2];
    strcpy(key,str.substr(str.size() - 1, str.size()).c_str());
    if(*key == '\n' && str.size() > 1) {
      this->GetDialog()->GetNickname()->\
          setPlainText(QString::fromStdString(str.substr(0,str.size() - 1)));
      emit this->GetDialog()->GetOK()->click();
    }
    if(*key == '27') {
      emit this->GetDialog()->GetCancel()->click();
    }
  }
  if(str.size() > MAX_NICKNAME_LEN) {
    this->GetDialog()->GetNickname()->\
        setPlainText(QString::fromStdString\
                     (str.substr(0,str.size() - 1)));
    this->GetDialog()->GetOK()->setEnabled(true);
    return;
  }
  if(!str.empty()) {
    this->GetDialog()->GetOK()->setEnabled(true);
  }
  else {
    this->GetDialog()->GetOK()->setEnabled(true);
  }
}

void Chat::TextSayChanged()
{
  string str = this->GetChatForm()->GetMessageEdit()->\
      toPlainText().toStdString();
  if(str.size() > MAX_NICKNAME_LEN) {
    this->GetChatForm()->GetMessageEdit()->\
        setPlainText(QString::fromStdString\
                     (str.substr(0,str.size() - 1)));
  }
}

void Chat::ChangeNicknameClick()
{
  this->change_dialog->setModal(true);
  this->change_dialog->show();
  this->change_dialog->GetNickname()->clear();
  this->change_dialog->GetNickname()\
      ->setPlaceholderText("Enter your new nickname");
}

void Chat::SayClient()
{
  QString say = this->chat_form->GetMessageEdit()->toPlainText();
  Message *say_message = new Message();
  string str;
  str.clear();
  say_message->InsertTimeAndAuthor(this->GetCurrentClient()->GetNickname());
  say_message->AddToMessage(say.toStdString());
  str = say_message->GetMyMessage();
  say_message->SetMessage("S");
  say_message->AddToMessage(say_message->GetLimiter());
  say_message->AddToMessage(\
        std::to_string(this->current_client->GetID()));
  say_message->AddToMessage(say_message->GetLimiter());
  say_message->AddToMessage(str);
  this->GetCurrentClient()->Send(say_message);
  delete say_message;
}

void Chat::LogInToChat()
{
  QString nickname = this->chat_form->GetChangeEdit()->toPlainText();
  Message *message = new Message();
  this->GetCurrentClient()->SetNickname(nickname.toStdString());
  message->SetMessage("L");
  message->AddToMessage(message->GetLimiter());
  message->AddToMessage(std::to_string(this->current_client->GetID()));
  message->AddToMessage(message->GetLimiter());
  message->AddToMessage(nickname.toStdString());
  this->current_client->DetermineIP(true);
  this->GetCurrentClient()->Send(message);
  delete message;
}

void Chat::ChangeNickname()
{
  QString new_nickname = this->change_dialog->GetNickname()\
      ->toPlainText();
  if(new_nickname.isEmpty()) {
    return;
  }
  Message *message = new Message("C");
  message->AddToMessage(message->GetLimiter());
  message->AddToMessage(std::to_string(this->current_client->GetID()));
  message->AddToMessage(message->GetLimiter());
  message->AddToMessage(new_nickname.toStdString());
  this->GetCurrentClient()->Send(message);
  delete message;
}

void Chat::Disconnect()
{
  QString nickname = this->GetChatForm()->GetChangeEdit()->toPlainText();
  Message *message = new Message("D");
  message->AddToMessage(message->GetLimiter());
  message->AddToMessage(std::to_string(this->current_client->GetID()));
  message->AddToMessage(message->GetLimiter());
  message->AddToMessage(nickname.toStdString());
  this->GetCurrentClient()->Send(message);
  delete message;
}


ChangeNicknameDialog* Chat::GetDialog() const
{
  return this->change_dialog;
}

void Chat::SetCurrentClient(Client *client)
{
  this->current_client = client;
}

Client* Chat::GetCurrentClient() const
{
  return this->current_client;
}

Client** Chat::GetClients() const
{
  return this->clients;
}

void Chat::SetClient(Client *client, int client_id)
{
  this->clients[client_id]->SetID(client->GetID());
  this->clients[client_id]->SetIP(client->GetIP());
  this->clients[client_id]->SetNickname(client->GetNickname());
  this->clients[client_id]->SetSocket(client->GetSocket());
}

int Chat::FindClient(Client *client)
{
  for(int i = 0; i < MAX_CLIENT_COUNT; i++) {
    if(this->clients[i]->GetID() == client->GetID()){
      return i;
    }
  }
  return NO_CLIENT;
}

void Chat::Accepting()
{
  const int kArgumentLen = 2;
  int is_server = 0;
  while(1) {
    Client *client = new Client();
    TCPSocket *tcpSocket = NULL;
    int arguments[kArgumentLen];
    tcpSocket = this->Accept();
    if(tcpSocket == NULL) {
      continue;
    }
    int client_id = this->FindClient(client);
    this->SetClientCount(this->GetClientCount() + 1);
    client->SetIP(tcpSocket->GetIP());
    client->SetSocket(tcpSocket->GetSocket());
    client->SetID(client_id);
    this->SetClient(client,client_id);
    _param.chat = this;
    _param.client_id = client_id;
#ifdef _WIN32
    /*this->clients[arguments[1]]->\
        SetClientHandle((HANDLE)CreateThread(0,0,\
                        (LPTHREAD_START_ROUTINE)this->\
                         ProcessingThreadFunction,\
                         LPVOID(this),0,0));*/
    this->clients[client_id]->\
            SetClientHandle((HANDLE)CreateThread(0,0,\
                            (LPTHREAD_START_ROUTINE)this->\
                             ProcessingThreadFunction,\
                             LPVOID(this),0,0));
#else
    int err = 0;
    pthread_t processing_thread;
    err = pthread_create(&processing_thread,\
                         NULL,&Chat::ProcessingThreadFunction,\
                         (void*)this);
            /*if (err != 0) {
              printf("\ncan't create thread :[%s]\n", strerror(err));
            }*/
    this->clients[arguments[1]]->SetClientHandle(processing_thread);
#endif
    delete client;
    delete tcpSocket;
  }
}

void Chat::ProcessingUserRequests(int client_id)
{
  char receive_buf[MAX_MESSAGE_LEN];
  const int kUtilityLetterLen = 2;
  char utility_letter[kUtilityLetterLen];
  int sender_id = NO_CLIENT;
  int client_position = NO_CLIENT;
  int count_package_recv = 1;
  memset(receive_buf,0,MAX_MESSAGE_LEN);
  memset(utility_letter,0,kUtilityLetterLen);
  while(CLIENT_SEND) {
    Message *message = new Message();
    while( message->GetMyMessage().empty()) {
      message->SetMessage("");
      memset(receive_buf,0,MAX_MESSAGE_LEN);
      this->clients[client_id]->\
                Receive(receive_buf,message,count_package_recv);
    }
    message->AddToMessage(receive_buf);
    message->ParseMessage(utility_letter,sender_id);
    /*  Message structure  */
    /*  S - Say_message(what client Say)  */
    /*  C - client whant Change nickname  */
    /*  L - Log in to chat  */
    /*  E - Error message  */
    /*  D - Disconnect message  */
    /*  F - client whant Find server  */
    switch(*utility_letter) {
      case 'S': {
        Message *send_message = new Message("S");
        send_message->AddToMessage(send_message->GetLimiter());
        send_message->AddToMessage(std::to_string(client_id));
        send_message->AddToMessage(send_message->GetLimiter());
        send_message->AddToMessage(message->GetMyMessage());
        this->SendToAll(send_message);
        delete send_message;
        break;
      }
      case 'C': {
        int new_position = this->FindNickname(message->GetMyMessage());
        if(new_position == NO_CLIENT) {
          string old_nickname = this->clients[client_id]->\
                                  GetNickname();
          this->SetClientNickname(message->GetMyMessage(),client_id);
          this->clients[client_id]->\
                        SetNickname(message->GetMyMessage());
          Message *send_message = new Message("C");
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage(std::to_string(client_id));
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage(message->GetMyMessage());
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage(old_nickname);
          this->SendToAll(send_message);
          delete send_message;
        }
        else {
          Message *send_message = new Message("E");
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage(std::to_string(client_id));
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage("Nickname already in use!");
          this->clients[client_id]->Send(send_message);
          delete send_message;
        }
        break;
      }
      case 'L': {
        string str = message->GetMyMessage();
        int new_position = this->FindNickname(str);
        if(new_position == NO_CLIENT && this->client_count < MAX_CLIENT_COUNT) {
          this->clients[client_id]->SetNickname(message->GetMyMessage());
          Message *send_message = new Message("L");
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage(std::to_string(this->clients[client_id]->GetID()));
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage(this->clients[client_id]->GetNickname());
          for(int i = 0; i < MAX_CLIENT_COUNT; i++) {
            if(this->clients[i]->GetID() == client_id) {
              continue;
            }
            else {
              send_message->AddToMessage(send_message->GetLimiter());
              send_message->AddToMessage(this->clients[i]->GetNickname());
            }
          }
          this->SendToAll(send_message);
          delete send_message;
        }
        else {
          Message *send_message = new Message("E");
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage(std::to_string(client_id));
          send_message->AddToMessage(send_message->GetLimiter());
          send_message->AddToMessage("Nickname already in use!");
          this->clients[client_id]->Send(send_message);
          delete send_message;
        }
        break;
      }
      case 'D': {
        Message *send_message = new Message("D");
        send_message->AddToMessage(send_message->GetLimiter());
        send_message->AddToMessage(std::to_string(this->clients[client_id]->GetID()));
        send_message->AddToMessage(send_message->GetLimiter());
        send_message->AddToMessage(this->clients[client_id]->GetNickname());
        this->SendToAll(send_message);
        this->clients[client_id]->SetSocket(SOCKET_ERROR);
        this->clients[client_id]->SetNickname("");
        this->clients[client_id]->SetID(NO_CLIENT);
        this->clients[client_id]->SetIP("");
        this->SetClientCount(this->GetClientCount() - 1);
        delete send_message;
        return;
      }
      case 'F': {
        Message *send_message = new Message("F");
        send_message->AddToMessage(send_message->GetLimiter());
        send_message->AddToMessage(std::to_string(this->clients[client_id]->GetID()));
        send_message->AddToMessage(send_message->GetLimiter());
        send_message->AddToMessage(this->GetIP());
        send_message->AddToMessage(send_message->GetLimiter());
        send_message->AddToMessage(this->GetChatName());
        this->clients[client_id]->Send(send_message);
        this->clients[client_id]->SetSocket(SOCKET_ERROR);
        this->clients[client_id]->SetNickname("");
        this->clients[client_id]->SetID(NO_CLIENT);
        this->clients[client_id]->SetIP("");
        this->SetClientCount(this->GetClientCount() - 1);
        delete send_message;
        return;
      }
    }
    delete message;
    memset(utility_letter,0,kUtilityLetterLen);
    memset(receive_buf,0,MAX_MESSAGE_LEN);
  }
}

void Chat::SendToAll(Message *message)
{
  for(int i = 0; i < MAX_CLIENT_COUNT; i++) {
    if(this->clients[i]->GetID() == NO_CLIENT) {
      continue;
    }
    this->clients[i]->Send(message);
  }
}



void Chat::Connecting(Message *_message)
{
  TCPSocket *tcpSocket = NULL;
  tcpSocket = this->current_client->\
      Connect(this->GetIP(),this->GetPort());
  tcpSocket->Send(_message);
  //shutdown(this->current_client->GetSocket(),SD_BOTH);
}

void Chat::Receiving(Client *client)
{
  int client_id = NO_CLIENT;
  const int kUtilityLetterLen = 2;
  char utility_letter[kUtilityLetterLen];
  char receive_buf[MAX_MESSAGE_LEN];
  int count_package_recv = 1;
  memset(receive_buf,0,MAX_MESSAGE_LEN);
  memset(utility_letter,0,kUtilityLetterLen);
  /*Error *error = new Error();
  error->SetCode(1);
  error->SetMessage("kl");
  emit error->Show();*/
  while(CLIENT_RECEIVE) {
    Message *receive_message = new Message();
    while(receive_message->GetMyMessage().empty()) {
      memset(receive_buf,0,MAX_MESSAGE_LEN);
      receive_message->SetMessage("");
      /*if(this->current_client->GetSocket() == SOCKET_ERROR) {
      continue;
      }
      else {
        this->current_client->Receive(receive_buf,\
                                    receive_message,\
                                    count_package_recv);
      }*/
      if(client->GetSocket() == SOCKET_ERROR) {
            continue;
            }
            else {
              client->Receive(receive_buf,\
                                          receive_message,\
                                          count_package_recv);
            }
    }

    receive_message->ParseMessage(utility_letter,client_id);
    switch(*utility_letter) {
      case 'S': {
        emit this->Saying(receive_message->GetMyMessage());
        break;
      }
      case 'C': {
        string old_nickname, new_nickname;
        old_nickname.clear();
        new_nickname.clear();
        std::size_t position = receive_message->\
            GetMyMessage().find(receive_message->GetLimiter());
        new_nickname = receive_message->\
            GetMyMessage().substr(0,position);
        old_nickname = receive_message->\
            GetMyMessage().substr(position + 1,\
                                  receive_message->GetMessageLen());
        emit this->Changing(old_nickname,new_nickname);
        break;
      }
      case 'L': {
        /*if(this->current_client->GetID() == NO_CLIENT) {
          this->current_client->SetID(client_id);
        }*/
        if(client->GetID() == NO_CLIENT) {
          client->SetID(client_id);
        }
        emit this->Logging(receive_message->GetMyMessage());
        break;
      }
      case 'D': {
        string message;
        if(client->GetID() == client_id) {
          message = "NO_MESSAGE";
        }
        else {
          message = "User \"" + \
              receive_message->GetMyMessage() + \
              "\" left the chat";
        }
        emit this->Disconnecting(receive_message->GetMyMessage(),message);
        /*if(this->current_client->GetID() == client_id) {
          //return;
        }*/
        if(client->GetID() == client_id) {
                  return;
        }
        break;
      }
      case 'F': {
        string name, addr;
        name.clear();
        addr.clear();
        std::size_t position = receive_message->\
            GetMyMessage().find(receive_message->GetLimiter());
        addr = receive_message->\
            GetMyMessage().substr(0,position);
        name = receive_message->\
            GetMyMessage().substr(position + 1,\
                                  receive_message->GetMessageLen());
        this->SetNameAndAddr(name,addr);
        emit this->SetAvailable(name);
        return;
        //break;
      }
    }
    delete receive_message;
    memset(receive_buf,0,MAX_MESSAGE_LEN);
    memset(utility_letter,0,kUtilityLetterLen);
    count_package_recv = 1;
  }
}

