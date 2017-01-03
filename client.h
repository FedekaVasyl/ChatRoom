#include "tcp_socket.h"
#include <string>
#include <iostream>

using std::string;

#ifndef CLIENT_H
#define CLIENT_H

#define NO_CLIENT -1
#define CLIENT_RECEIVE 1
#define CLIENT_SEND 1

class Client : public TCPSocket
{
private:
  int id;
  string nickname;
#ifdef _WIN32
  HANDLE client_handle;
#else
  pthread_t client_handle;
#endif
public:
  Client();
  Client(string _nickname);
  Client(const Client &client);
  ~Client();
  TCPSocket* Connect(string _address, int _port);
  void Disconnect();
  string GetNickname() const;
  void SetNickname(string _nickname);
  void SetID(int _id);
  int GetID() const;
#ifdef _WIN32
  HANDLE GetClientHandle() const;
  void SetClientHandle(HANDLE _handle);
#else
  pthread_t GetClientHandle() const;
  void SetClientHandle(pthread_t _handle);
#endif
  friend bool operator==(const Client& left, const Client& right);
};

#endif // CLIENT_H
