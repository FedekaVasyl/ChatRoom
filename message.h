#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <ws2def.h>
  #include <ws2tcpip.h>
  #include <Windows.h>
  #include <process.h>
  #include <conio.h>
#else
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <pthread.h>
#endif

#include "my_time.h"
#include <QObject>

using std::string;


class Message : public QObject
{
  Q_OBJECT
private:
  string message;
  int message_len;
  MyTime *time;
  const string kDelimiter;
  const string kLimiter;
public:
  Message() : kDelimiter("\r"), kLimiter("*")
  {
    message.clear();
    message_len = 0;
    time = new MyTime();
  }
  Message(string _message) : kDelimiter("\n\r"), kLimiter("*")
  {
    message.clear();
    message = _message;
    message_len = _message.size();
    time = new MyTime();
  }
  virtual ~Message();
  string GetMyMessage() const;
  int GetMessageLen() const;
  void SetMessage(string _message);
  void SetMessageLen(int _len);
  void AddToMessage(string _message);
  MyTime* GetMyTime() const;
  void InsertTimeAndAuthor(string _author);
  const string GetLimiter() const;
  void ParseMessage(char *utility_letter, int &id);
};

#endif // MESSAGE_H
