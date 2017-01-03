//#include "mainwindow.h"
#include "tcp_socket.h"
#include "error.h"
#include <cerrno>
#include "server.h"

Server::Server()
{
  this->server_handle = 0;
  this->is_started = false;
}

Server::Server(string _address)
{
  this->SetIP(_address);
  this->server_handle = 0;
  this->is_started = false;
}

Server::~Server()
{
#ifdef _WIN32
  TerminateThread(this->server_handle, NULL);
  CloseHandle(this->server_handle);
#else
  pthread_cancel(this->server_handle);
#endif
}
#ifdef _WIN32
  void Server::SetServerHandle(HANDLE _handle)
  {
    this->server_handle = _handle;
  }

  HANDLE Server::GetServerHandle() const
  {
    return this->server_handle;
  }
#else
  void Server::SetServerHandle(pthread_t _handle)
  {
    this->server_handle = _handle;
  }

  pthread_t Server::GetServerHandle() const
  {
    return this->server_handle;
  }
#endif
bool Server::GetIsStarted() const
{
  return this->is_started;
}

void Server::SetIsStarted(bool _is_started)
{
  this->is_started = _is_started;
}

int Server::ServerStartup()
{
  int result = 0;
  const int kBase = 10, kPortLen = 5;
  struct addrinfo addr_template;
  struct addrinfo* server_addr = NULL;
#ifdef _WIN32
  PCSTR Host = 0;
  this->DetermineIP(true);
  string str = this->GetIP();
  PCSTR Port = 0;
  char buf[kPortLen];
  memset(buf,0,kPortLen);
  itoa(this->GetPort(),buf,kBase);
  Port = buf;
  Host = str.c_str();
#else
  const int kHostLen = 15;
  char Host[kHostLen];
  char Port[kPortLen];
  memset(Port,0,kPortLen);
  memset(Host,0,kHostLen);
  this->DetermineIP(true);
  strcpy(Host,this->GetIP().c_str());
  sprintf(Port,"%d",this->GetPort());
#endif
  memset(&addr_template,0,sizeof(addr_template));
  addr_template.ai_family = AF_INET;
  addr_template.ai_socktype = SOCK_STREAM;
  addr_template.ai_protocol = IPPROTO_TCP;
  addr_template.ai_flags = AI_PASSIVE;
  result = getaddrinfo(Host, Port, &addr_template, &server_addr);
  if (result != 0) {
    this->SockExit();
#ifdef _WIN32
    Error *error = new Error();
    error->SetCode((int) WSAGetLastError());
#else
    Error *error = new Error((int)errno);
#endif
    error->SetMessage("ServerStartup() failed");
    emit error->Show();
    delete error;
    return false;
  }
  /* Create the socket */
  this->SetSocket(socket(server_addr->ai_family,      \
                         server_addr->ai_socktype,    \
                         server_addr->ai_protocol));
  if (this->GetSocket() == INVALID_SOCKET) {
    freeaddrinfo(server_addr);
    this->SockExit();
#ifdef _WIN32
    Error *error = new Error();
    error->SetCode((int) WSAGetLastError());
#else
    Error *error = new Error((int)errno);
#endif
    error->SetMessage("ServerStartup() failed");
    emit error->Show();
    delete error;
    return false;
  }
  if (!this->SetParam()) {
    return false;
  }
  if (!this->Bind(server_addr) || !this->Listen()) {
    return false;
  }
  this->SetIsStarted(true);
  return true;
}

/* Bind the socket to the IP-address */
bool Server::Bind(struct addrinfo* server_addr)
{
  int result = 0;
  result = bind(this->GetSocket(), server_addr->ai_addr, \
      (int)server_addr->ai_addrlen);
  if (result == SOCKET_ERROR) {
    freeaddrinfo(server_addr);
    StopServer();
#ifdef _WIN32
    int ss = WSAGetLastError();
    Error *error = new Error((int) WSAGetLastError());
    //error->SetCode();
#else
    Error *error = new Error((int)errno);
#endif
    error->SetMessage("Bind() failed");
    //string ss = strerror(errno);
    //error->SetMessage(strerror(errno));
    emit error->Show();
    delete error;
    return false;
  }
  return true;
}

/* Initialize the listening socket */
bool Server::Listen()
{
  int result = 0;
  result = listen(this->GetSocket(), SOMAXCONN);
  if (result == SOCKET_ERROR) {
    StopServer();
#ifdef _WIN32
    Error *error = new Error((int)WSAGetLastError());
#else
    Error *error = new Error((int)errno);
#endif
    error->SetMessage("Listen() failed");
    emit error->Show();
    delete error;
    return false;
  }
  return true;
}

void Server::StopServer()
{
this->SockExit();
}

TCPSocket* Server::Accept()
{
  struct sockaddr_in address;
  socklen_t len = sizeof(address);
  memset(&address, 0, len);
  int _socket = accept(this->GetSocket(), \
                       (struct sockaddr*)&address,&len);
  if (_socket == SOCKET_ERROR) {
    return NULL;
  }
  sockaddr_in name;
#ifdef _WIN32
  int n_l = sizeof (name);
  ZeroMemory (&name, sizeof (name));
#else
  socklen_t n_l = sizeof(name);
  memset(&name,0,sizeof(name));
#endif
  if(getpeername(_socket,(sockaddr*)&name,&n_l)\
     == SOCKET_ERROR) {
    return NULL;
  }
  return new TCPSocket(_socket, &address);
}


