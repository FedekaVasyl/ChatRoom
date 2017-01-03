#include "tcp_socket.h"
#ifndef SERVER_H
#define SERVER_H


class Server : public TCPSocket
{
private:
#ifdef _WIN32
  HANDLE server_handle;
#else
  pthread_t server_handle;
#endif
  bool is_started;
public:
  Server();
  Server(string _address);
  virtual ~Server();
  TCPSocket* Accept();
#ifdef _WIN32
  HANDLE GetServerHandle() const;
  void SetServerHandle(HANDLE _handle);
#else
  pthread_t GetServerHandle() const;
  void SetServerHandle(pthread_t _handle);
#endif
  bool GetIsStarted() const;
  void SetIsStarted(bool _is_started);
  bool Bind(struct addrinfo* server_addr);
  void CreateChat();
  bool Listen();
  void ScanLocalNetwork();
  int ServerStartup();
  void StopServer(); 
};

#endif // SERVER_H
