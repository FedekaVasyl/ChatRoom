#include "client.h"
#include "error.h"

Client::Client()
{
  this->nickname.clear();
  this->id = NO_CLIENT;
#ifdef _WIN32
  this->client_handle = INVALID_HANDLE_VALUE;
#else
  this->client_handle = 0;
#endif
}

Client::Client(string _nickname)
{
  this->nickname = _nickname;
  this->id = NO_CLIENT;
#ifdef _WIN32
  this->client_handle = INVALID_HANDLE_VALUE;
#else
  this->client_handle = 0;
#endif
}

Client::Client(const Client &client) {
  this->SetID(client.GetID());
  this->SetIP(client.GetIP());
  this->SetNickname(client.GetNickname());
  this->SetSocket(client.GetSocket());
}

Client::~Client()
{
#ifdef _WIN32
  if(this->client_handle != INVALID_HANDLE_VALUE) {
    TerminateThread(this->client_handle, NULL);
    CloseHandle(this->client_handle);
  }
#else
  if(this->client_handle != 0) {
      pthread_cancel(this->client_handle);
  }
#endif
}

string Client::GetNickname() const
{
  return this->nickname;
}

void Client::SetNickname(string _nickname)
{
  this->nickname = _nickname;
}

TCPSocket* Client::Connect(string _address, int _port)
{
  struct sockaddr_in addr;
  int _socket;
  int result;
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if(_socket == SOCKET_ERROR) {
    Error *error = new Error();
#ifdef _WIN32
    error->SetCode((int) WSAGetLastError());
#else
    error->SetCode((int)errno);
#endif
    error->SetMessage("socket() failed!");
    emit error->Show();
    delete error;
    return NULL;
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(_address.c_str());
  addr.sin_port = htons(_port);
#ifdef _WIN32
  result = ::connect(_socket,(SOCKADDR*)&addr,sizeof(addr));
#else
  result = ::connect(_socket,(sockaddr*)&addr,sizeof(addr));
#endif
  /*if(result < 0) {
    //return NULL;
    int er = WSAGetLastError();
    er++;
  }*/
  this->SetSocket(_socket);
  if(result){
#ifdef _WIN32
    closesocket(_socket);
#else
    shutdown(_socket,2);
#endif
    return NULL;
  }
  return new TCPSocket(_socket,&addr);
}

bool operator==(const Client& left, const Client& right)
{
  if(left.GetID() == right.GetID()){
    return true;
  }
  else {
    return false;
  }
}

void Client::SetID(int _id)
{
  this->id = _id;
}

int Client::GetID() const
{
  return this->id;
}
#ifdef _WIN32
  HANDLE Client::GetClientHandle() const
  {
    return this->client_handle;
  }

  void Client::SetClientHandle(HANDLE _handle)
  {
    this->client_handle = _handle;
  }
#else
  pthread_t Client::GetClientHandle() const
  {
    return this->client_handle;
  }

void Client::SetClientHandle(pthread_t _handle)
  {
    this->client_handle = _handle;
  }
#endif
