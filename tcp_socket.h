
#pragma comment(lib, "IPHLPAPI.lib")
#ifndef TCPSocket_H
#define TCPSocket_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iterator>
#include <iostream>
#include <errno.h>
#include "message.h"

#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

//#ifdef
#include <stdio.h>
#include <assert.h>
//#include <windows.h>
//#include <lm.h>

#define MAX_CLIENT_COUNT 20
#define MAX_SERVER_COUNT 200
#define MAX_NICKNAME_LEN 30
#define MAX_SAY_LEN 950
#define MAX_MESSAGE_LEN  1000
#define xstr(s) str(s)
#define str(s) #s

#define ARP_CACHE       "/proc/net/arp"
#define ARP_STRING_LEN  1023
#define ARP_BUFFER_LEN  (ARP_STRING_LEN + 1)

/* Format for fscanf() to read the 1st, 4th, and 6th space-delimited fields */
#define ARP_LINE_FORMAT "%" xstr(ARP_STRING_LEN) "s %*s %*s " \
                        "%" xstr(ARP_STRING_LEN) "s %*s " \
                        "%" xstr(ARP_STRING_LEN) "s"

#include <QObject>
#include <QTimer>

#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <ws2def.h>
  #include <ws2tcpip.h>
  #include <iphlpapi.h>
  #include <Windows.h>
  #include <process.h>
  #include <conio.h>
  #include <lm.h>
#else
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <pthread.h>
  #include <limits.h>     
  #include <sys/types.h>
  #include <ifaddrs.h>
  #include <netinet/in.h>
  #include <net/if.h>
#endif


#undef INVALID_SOCKET
#define INVALID_SOCKET -1
#undef SOCKET_ERROR
#define SOCKET_ERROR -1
#define NO_LAN_NAME -1
#define APPLICATION_RUNNING 1

#pragma comment(lib,"Ws2_32.lib")

using namespace std;


class TCPSocket : public QObject
{
  Q_OBJECT
signals:
  void ShowAvailableChat();
private:
#ifdef _WIN32
  HANDLE finding_handle;
#else
  pthread_t finding_handle;
#endif
  string ip_address;
  const int kPort;
  volatile int socket_descriptor;
  string *lan_addr; // IP addresses of all computers on the LAN
  string *lan_name; //Names of servers that run on computers on the LAN
  int lan_count;  //Number of all computers on the LAN
  QTimer *timer;
public:
  TCPSocket();
  TCPSocket(int _socket, struct sockaddr_in* _address);
  virtual ~TCPSocket();
  string GetIP() const;
  const int GetPort() const;
  int GetSocket() const;
  int GetLanCount() const;
  string* GetLanAddr() const;
  string GetLanAddr(int _position) const;
  string* GetLanName() const;
  string GetLanName(int _position) const;
  void CreateLanAddr();
  void SetLanAddr(string _addr, int _position);
  void CreateLanName();
  void SetLanName(string _name, int _position);
  void SetNameAndAddr(string _name, string _addr);
  void SetLanCount(int _count);
  void SetIP(string _ip);
  void SetSocket(int _socket);
  int FindLanName(string _name);
  void Update();
  void Send(Message* _message);
  int Receive(char* receive_buf,\
               Message* _message,\
               int count_package_recv);
  string DetermineIP(bool is_current, char* computer_name = 0);
  void FindComputers();
  bool SetParam();
#ifdef _WIN32
  bool SockInit();
  void SetMutexHandle(HANDLE _handle);
  HANDLE GetMutexHandle() const;
  HANDLE GetFindingHandle() const;
  void SetFindingHandle(HANDLE _handle);
#else
  void SetMutexHandle(pthread_t _handle);
  pthread_t GetMutexHandle() const;
  pthread_t GetFindingHandle() const;
  void SetFindingHandle(pthread_t _handle);
#endif
  void SockExit();
  QTimer* GetTimer() const;
};

#endif // TCPSocket_H
