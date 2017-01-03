#ifndef MY_TIME_H
#define MY_TIME_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iterator>
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

using std::string;

class MyTime
{
private:
  int day;
  int hour;
  int minute;
  int month;
  int second;
  int year;
public:
  MyTime();
  ~MyTime();
  int GetDay() const;
  int GetHour() const;
  int GetMinute() const;
  int GetMonth() const;
  int GetSecond() const;
  int GetYear() const;
  void SetDay(int _day);
  void SetHour(int _hour);
  void SetMinute(int _minute);
  void SetMonth(int _month);
  void SetSecond(int _second);
  void SetYear(int _year);
  string NormalForm();
  void FindTime(string &_time);
  void FindDate(string &_date);
};

#endif // MY_TIME_H
