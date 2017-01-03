#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->connect_button->setEnabled(false);
  ui->create_button->setEnabled(false);
  ui->selected_chat->setPlaceholderText("No chat available");
  ui->chat_name->setPlaceholderText("Enter chat name");
  this->connect_button = ui->connect_button;
  this->create_button = ui->create_button;
  this->update_button = ui->update_button;
  this->selected_chat = ui->selected_chat;
  this->create_chat = ui->chat_name;
  this->available_chat = ui->available_chat;
  this->update_progress_bar = ui->update_progress_bar;
  this->list_model = new QStringListModel(this);
  QStringList data_list;
  this->list_model->setStringList(data_list);
  ui->available_chat->setModel(this->list_model);

  this->chat = new Chat();
  this->chat->DetermineIP(true);
#ifdef _WIN32
  if (!this->chat->SockInit()) {
    Error *error = new Error(errno);
    error->SetMessage("ServerStartup() failed");
    emit error->Show();
    delete error;
    return;
  }
#endif
  this->chat->GetCurrentClient()->\
        Connect(this->chat->GetIP(),this->chat->GetPort());
  this->chat->_receive.chat = this->chat;
  this->chat->_receive.client = this->chat->GetCurrentClient();
#ifdef _WIN32
  this->chat->SetReceivingHandle((HANDLE) CreateThread(0,0,\
       (LPTHREAD_START_ROUTINE)this->chat->ReceiveThreadFunction,\
                                           LPVOID(this->chat),0,0));
#else
  int err = 0;
  pthread_t _thread;
  err = pthread_create(&_thread,NULL,&Chat::ReceiveThreadFunction,\
                                            (void*)this->chat);
  this->chat->SetReceivingHandle(_thread);
#endif
  /*QObject::connect(this->available_chat,\
                   SIGNAL(doubleClicked(QModelIndex)),\
                   this->connect_button,\
                   SIGNAL(clicked(bool)));*/
  QObject::connect(ui->connect_button,\
                   SIGNAL(clicked(bool)),this,SLOT(ConnectToChat()));
  QObject::connect(ui->create_button,\
                   SIGNAL(clicked(bool)),this,SLOT(CreateNewChat()));
  QObject::connect(this->available_chat,\
                   SIGNAL(clicked(QModelIndex)),this,\
                   SLOT(ConnectEnable(QModelIndex)));
  QObject::connect(this->create_chat,\
                   SIGNAL(textChanged()),this,SLOT(CreateEnable()));
  QObject::connect(this->chat->GetChatForm()->GetLeaveChat(),\
                   SIGNAL(clicked(bool)),this,SLOT(ShowMainForm()));
  QObject::connect(this->update_button,\
                   SIGNAL(clicked(bool)),this,SLOT(UpdateClicked()));
  QObject::connect(this->chat->GetTimer(),\
                   SIGNAL(timeout()),this,SLOT(TimerOverFlow()),\
                   Qt::ConnectionType::DirectConnection);
  QObject::connect(this->chat,\
                   SIGNAL(Logging(string)),this,SLOT(Logging(string)),\
                   Qt::ConnectionType::BlockingQueuedConnection);
  QObject::connect(this->chat,\
                   SIGNAL(Saying(string)),this,SLOT(Saying(string)),\
                   Qt::ConnectionType::BlockingQueuedConnection);
  QObject::connect(this->chat,\
                   SIGNAL(Changing(string,string)),this,\
                   SLOT(Changing(string,string)),\
                   Qt::ConnectionType::BlockingQueuedConnection);
  QObject::connect(this->chat,\
                   SIGNAL(Disconnecting(string,string)),this,\
                   SLOT(Disconnecting(string,string)),\
                   Qt::ConnectionType::BlockingQueuedConnection);
  QObject::connect(this->chat,\
                   SIGNAL(SetAvailable(string)),this,\
                   SLOT(SetAvailable(string)),\
                   Qt::ConnectionType::BlockingQueuedConnection);
  QObject::connect(this->chat,\
                   SIGNAL(ShowAvailableChat()),this,\
                   SLOT(ShowAvailableChat()),\
                   Qt::ConnectionType::BlockingQueuedConnection);

  /*this->chat->FindComputers();
  this->ShowAvailableChat();*/
}

MainWindow::~MainWindow()
{
  delete ui;
  delete connect_button;
  delete create_button;
  delete selected_chat;
  delete create_chat;
  delete available_chat;
  delete list_model;
}


std::string MainWindow::GetSelectedChatText() const
{
  return ui->selected_chat->toPlainText().toStdString();
}

std::string MainWindow::GetChatNameText() const
{
  return ui->chat_name->toPlainText().toStdString();
}

Chat* MainWindow::GetChat() const
{
  return this->chat;
}

void MainWindow::CreateNewChat()
{
  string chat_name = this->GetChatNameText();

  if(this->chat->FindLanName(chat_name) != NO_LAN_NAME) {
    Error *error = new Error(NO_LAN_NAME);
    error->SetMessage("Chat with the same name already exists!");
    emit error->Show();
    delete error;
    return;
  }
  this->chat->SetCurrentClient(new Client());

  this->chat->SetChatName(chat_name);
  if(!this->chat->GetIsStarted()) {
    this->chat->ServerStartup();
#ifdef _WIN32
    this->chat->SetServerHandle((HANDLE)CreateThread(0,0,\
         (LPTHREAD_START_ROUTINE)this->chat->AcceptThreadFunction,\
                                             LPVOID(this->chat),0,0));
#else
    int err = 0;
    pthread_t server_thread;
    err = pthread_create(&server_thread,NULL,&Chat::AcceptThreadFunction,\
                                                    (void*)this->chat);
    this->chat->SetServerHandle(server_thread);
#endif
  }
  this->chat->GetCurrentClient()->\
        Connect(this->chat->GetIP(),this->chat->GetPort());
  this->chat->_receive.chat = this->chat;
  this->chat->_receive.client = this->chat->GetCurrentClient();
#ifdef _WIN32
  this->chat->SetReceivingHandle((HANDLE) CreateThread(0,0,\
       (LPTHREAD_START_ROUTINE)this->chat->ReceiveThreadFunction,\
                                           LPVOID(this->chat),0,0));
#else
  int err = 0;
  pthread_t recv_thread;
  err = pthread_create(&recv_thread,NULL,&Chat::ReceiveThreadFunction,\
                                                (void*)this->chat);
  this->chat->SetReceivingHandle(recv_thread);
  /*if(err != 0) {
    printf("\ncan't create thread :[%s]\n", strerror(err));
  }*/
#endif

  this->hide();
  emit this->chat->ShowChatForm();
  this->chat->GetChatForm()->GetChangeEdit()->setText("server");
  emit this->chat->GetChatForm()->GetLogIn()->click();
  this->chat->GetChatForm()->GetChatNameLable()->\
      setText(QString::fromStdString(this->chat->GetChatName()));
}

void MainWindow::ConnectToChat()
{
  string chat_name = this->GetSelectedChat()->\
      toPlainText().toStdString();
  int position = this->chat->FindLanName(chat_name);
  if(position == NO_LAN_NAME) {
    Error *error = new Error(NO_LAN_NAME);
    error->SetMessage("This chat does not exist!");
    emit error->Show();
    delete error;
    return;
  }
  this->chat->SetIP(this->chat->GetLanAddr(position));
  this->chat->SetCurrentClient(new Client());
  this->chat->GetCurrentClient()->\
        Connect(this->chat->GetIP(),this->chat->GetPort());
  this->chat->_receive.chat = this->chat;
  this->chat->_receive.client = this->chat->GetCurrentClient();
#ifdef _WIN32
  this->chat->SetReceivingHandle((HANDLE) CreateThread(0,0,\
     (LPTHREAD_START_ROUTINE)this->chat->ReceiveThreadFunction,\
                                         LPVOID(this->chat),0,0));
#else
  int err = 0;
  pthread_t _thread;
  err = pthread_create(&_thread,NULL,&Chat::ReceiveThreadFunction,\
                                            (void*)this->chat);
  this->chat->SetReceivingHandle(_thread);

#endif
  this->hide();
  emit this->chat->ShowChatForm();
  //this->chat->GetChatForm()->GetChangeEdit()->setText("guest");
  //emit this->chat->GetChatForm()->GetLogIn()->click();
  this->chat->GetChatForm()->GetChatNameLable()->\
      setText(QString::fromStdString(this->chat->GetChatName()));
}

void MainWindow::ShowMainForm()
{
  this->GetCreateChat()->clear();
  this->GetCreateChat()->setPlaceholderText("Enter chat name");
  this->GetSelectedChat()->clear();
  this->GetSelectedChat()->setPlaceholderText("No chat available");
  this->available_chat->setUpdatesEnabled(false);
  for(int i = 0; i < this->list_model->rowCount(); i++) {
    this->list_model->removeRow(i);
  }
  this->available_chat->setUpdatesEnabled(true);
}

QPushButton* MainWindow::GetConnectButton() const
{
  return this->connect_button;
}

QPushButton* MainWindow::GetCreateButton() const
{
  return this->create_button;
}

QPushButton* MainWindow::GetUpdateButton() const
{
  return this->update_button;
}

QTextEdit* MainWindow::GetSelectedChat() const
{
  return this->selected_chat;
}

QTextEdit* MainWindow::GetCreateChat() const
{
  return this->create_chat;
}

QListView* MainWindow::GetAvailableChat() const
{
  return this->available_chat;
}

QStringListModel* MainWindow::GetListModel() const
{
  return this->list_model;
}

QProgressBar* MainWindow::GetUpdateProgressBar() const
{
  return this->update_progress_bar;
}

void MainWindow::ConnectEnable(QModelIndex _index)
{
  QString str = this->list_model->\
      index(_index.row(),0).data(Qt::DisplayRole).toString();
  this->selected_chat->setText(str);
  this->connect_button->setEnabled(true);
}

void MainWindow::CreateEnable()
{
  string str = this->create_chat->toPlainText().toStdString();
  if(!str.empty()) {
    char key[2];
    strcpy(key,str.substr(str.size() - 1, str.size()).c_str());
    if(*key == '\n' && str.size() > 1) {
      emit this->create_button->click();
    }
  }
  if(str.size() > MAX_CHAT_NAME_LEN) {
    this->create_chat->\
        setPlainText(QString::fromStdString\
                     (str.substr(0,str.size() - 1)));
    //this->create_button->setEnabled(true);
    return;
  }
  if(!this->create_chat->toPlainText().isEmpty()) {
    this->create_button->setEnabled(true);
  }
  else {
    this->create_button->setEnabled(false);
  }
}

void MainWindow::TimerOverFlow()
{
  int value = this->update_progress_bar->value();
  if(value < 98) {
    this->update_progress_bar->setValue(value + 1);
    this->update_button->setEnabled(false);
    //this->chat->GetTimer()->setInterval(600);
    //this->chat->GetTimer()->start();
  }
  else if(value < 100 && value > 98) {
    this->update_progress_bar->setValue(value + 1);
    this->chat->GetTimer()->setInterval(1200);
    //this->chat->GetTimer()->start();
  }
  else {
    this->chat->GetTimer()->stop();
    this->update_progress_bar->setValue(100);
    this->update_button->setEnabled(true);
  }
}

void MainWindow::UpdateClicked()
{
  this->update_progress_bar->setValue(0);
  this->chat->StartFinding();
}

void MainWindow::Logging(string _str)
{
  this->chat->GetChatForm()->UpdaiteClientList(_str);
  this->chat->GetChatForm()->GetChangeNickname()->setEnabled(true);
  this->chat->GetChatForm()->GetChatBody()->setEnabled(true);
  this->chat->GetChatForm()->GetMessageEdit()->setEnabled(true);
  this->chat->GetChatForm()->GetSay()->setEnabled(true);
  this->chat->GetChatForm()->GetChangeEdit()->setEnabled(false);
  this->chat->GetChatForm()->GetLogIn()->setEnabled(false);
  Message *message = new Message();
  message->InsertTimeAndAuthor("SERVER");
  std::size_t position = _str.find(message->GetLimiter());
  string str;
  str.clear();
  str = _str.substr(0,position);
  message->AddToMessage("User \"" +\
                        str + "\" just joined the chat");
  this->chat->GetChatForm()->GetChatBody()->\
      append(QString::fromStdString(message->GetMyMessage()));
  if(this->chat->GetCurrentClient()->GetNickname().empty()) {
    this->chat->GetChatForm()->GetChangeEdit()->\
          setPlainText(QString::fromStdString(str));
    this->chat->GetCurrentClient()->SetNickname(str);
  }
  delete message;
}

void MainWindow::Saying(string _str)
{
  this->chat->GetChatForm()->GetChatBody()->\
      append(QString::fromStdString(_str));
  this->chat->GetChatForm()->GetMessageEdit()->clear();
}

void MainWindow::Changing(string old_nickname, string new_nickname)
{
  QAbstractItemModel* model = this->chat->GetChatForm()->\
      GetClientList()->model();
  /*if(this->chat->GetChatForm()->GetChangeEdit()->\
     toPlainText().toStdString() != old_nickname) {
    this->chat->GetChatForm()->GetChangeEdit()->setText\
        (QString::fromStdString(new_nickname));
  }*/
  QString str;
  Message *message = new Message();
  message->InsertTimeAndAuthor("SERVER");
  message->AddToMessage("User \"" + \
                        old_nickname + \
                        "\" changed his nickname to \"" + \
                        new_nickname + "\"");
  //this->chat->GetChatForm()->GetChatBody()->append\
         (QString::fromStdString(message->));
  this->chat->GetChatForm()->GetChatBody()->append\
       (QString::fromStdString(message->GetMyMessage()));
  if(this->chat->GetCurrentClient()->GetNickname() == old_nickname) {
    this->chat->GetCurrentClient()->SetNickname(new_nickname);
    this->chat->GetChatForm()->GetChangeEdit()->setText\
          (QString::fromStdString(new_nickname));
  }
  this->chat->GetChatForm()->\
        GetClientList()->setUpdatesEnabled(false);
  for(int i = 0; i < model->rowCount(); i++) {
    str = model->index(i,0).data(Qt::DisplayRole).toString();
    if(str.toStdString() == old_nickname) {
      model->removeRow(i);
      this->chat->GetChatForm()->SetIntoClientList(new_nickname);
      break;
    }
  }
  this->chat->GetChatForm()->\
        GetClientList()->setUpdatesEnabled(true);
  delete message;
}

void MainWindow::Disconnecting(string nickname, string message)
{
  QAbstractItemModel* model = this->chat->GetChatForm()->\
      GetClientList()->model();
  QString str;
  this->chat->GetChatForm()->\
        GetClientList()->setUpdatesEnabled(false);
  for(int i = 0; i < model->rowCount(); i++) {
    str = model->index(i,0).data(Qt::DisplayRole).toString();
    if(str == QString::fromStdString(nickname)) {
      model->removeRow(i);
    }
  }
  this->chat->GetChatForm()->\
        GetClientList()->setUpdatesEnabled(true);
  if(message == "NO_MESSAGE") {
    this->show();
    this->chat->GetChatForm()->hide();
    this->chat->GetChatForm()->GetChangeEdit()->setEnabled(false);
    this->chat->GetChatForm()->GetChangeEdit()->clear();
    this->chat->GetChatForm()->GetChangeNickname()->setEnabled(false);
    this->chat->GetChatForm()->GetChatBody()->clear();
    this->chat->GetChatForm()->GetLeaveChat()->setEnabled(false);
    this->chat->GetChatForm()->GetLogIn()->setEnabled(false);
    this->chat->GetChatForm()->GetMessageEdit()->setEnabled(false);
    this->chat->GetChatForm()->GetMessageEdit()->clear();
    this->chat->GetChatForm()->GetSay()->setEnabled(false);
    QAbstractItemModel* model = this->chat->GetChatForm()->\
        GetClientList()->model();
    this->chat->GetChatForm()->GetClientList()->\
        setUpdatesEnabled(false);
    for(int i = 0; i < model->rowCount(); i++) {
      model->removeRow(i);
    }
    this->chat->GetChatForm()->GetClientList()->\
        setUpdatesEnabled(true);
  }
  else {
    Message *_message = new Message();
    _message->InsertTimeAndAuthor("SERVER");
    _message->AddToMessage(message);
    this->chat->GetChatForm()->GetChatBody()->\
        append(QString::fromStdString(_message->GetMyMessage()));
    delete _message;
  }
  //this->UpdateAvailable();
}

void MainWindow::ShowAvailableChat()
{

  Client *founder = new Client();
  this->chat->_receive.chat = this->chat;
  this->chat->_receive.client = founder;
#ifdef _WIN32
    founder->SetClientHandle((HANDLE) CreateThread(0,0,\
       (LPTHREAD_START_ROUTINE)this->chat->ReceiveThreadFunction,\
                                           LPVOID(this->chat),0,0));
#else
    int err = 0;
    pthread_t _thread;
    err = pthread_create(&_thread,NULL,&Chat::ReceiveThreadFunction,NULL);
    founder->SetClientHandle(_thread);
#endif
          string str;
  QAbstractItemModel* model = this->GetAvailableChat()->model();

  this->GetAvailableChat()->setUpdatesEnabled(false);
  for(int i = 0; i < model->rowCount(); i++) {
    model->removeRow(i);
  }
  this->GetAvailableChat()->setUpdatesEnabled(true);
  for(int i = 0; i < this->chat->GetLanCount(); i++) {
    /*if(this->chat->GetCurrentClient()->\
             Connect(this->chat->GetLanAddr(i),\
                     this->chat->GetPort()) == NULL) {*/
    if(this->chat->GetLanAddr(i).empty() || \
            founder->Connect(this->chat->GetLanAddr(i),\
                             this->chat->GetPort()) == NULL) {
      str = this->chat->GetLanAddr(i);
      continue;
    }

    Message *message = new Message("F");
    message->AddToMessage(message->GetLimiter());
    message->AddToMessage(std::to_string(this->chat->\
                                         GetCurrentClient()->GetID()));
    message->AddToMessage(message->GetLimiter());
    message->AddToMessage("Find");
    founder->Send(message);
    delete message;
  }
  //delete founder;
}

void MainWindow::SetAvailable(string _name)
{
  this->list_model->insertRow(this->list_model->rowCount());
  QModelIndex index = this->list_model->\
      index(this->list_model->rowCount() - 1);
  this->list_model->setData(index,QString::fromStdString(_name));
  this->available_chat->setUpdatesEnabled(true);
}

void MainWindow::UpdateAvailable()
{
  this->chat->Update();
  //this->chat->FindComputers();
  this->chat->StartFinding();
  this->ShowAvailableChat();
  this->chat->GetCurrentClient()->\
        Connect(this->chat->GetIP(),this->chat->GetPort());
}
