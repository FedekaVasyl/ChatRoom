#include "my_time.h"

MyTime::MyTime()
{
  time_t raw_time;
  struct tm *timeinfo;
  time(&raw_time);
  timeinfo = localtime(&raw_time);
  this->second = timeinfo->tm_sec;
  this->minute = timeinfo->tm_min;
  this->hour = timeinfo->tm_hour;
  this->day = timeinfo->tm_mday;
  this->month = timeinfo->tm_mon + 1;       /*Corrects month value*/
  this->year = timeinfo->tm_year + 1900;    /*Corrects year value*/
}

MyTime::~MyTime()
{
}

int MyTime::GetDay() const
{
  return this->day;
}

int MyTime::GetHour() const
{
  return this->hour;
}

int MyTime::GetMinute() const
{
  return this->minute;
}

int MyTime::GetMonth() const
{
  return this->month;
}

int MyTime::GetSecond() const
{
  return this->second;
}

int MyTime::GetYear() const
{
  return this->year;
}

void MyTime::SetDay(int _day)
{
  this->day = _day;
}

void MyTime::SetHour(int _hour)
{
  this->hour = _hour;
}

void MyTime::SetMinute(int _minute)
{
  this->minute = _minute;
}

void MyTime::SetMonth(int _month)
{
  this->month = _month;
}

void MyTime::SetSecond(int _second)
{
  this->second = _second;
}

void MyTime::SetYear(int _year)
{
  this->year = _year;
}

string MyTime::NormalForm()
{
  /*The normal form of date and time: hh:mm:ss | dd/mn/year*/
  string new_date, new_time, normal_form;
  new_date.clear();
  new_time.clear();
  normal_form.clear();
  this->FindDate(new_date);
  this->FindTime(new_time);
  normal_form = new_time;
  normal_form += " | ";
  normal_form += new_date;
  return normal_form;
}

void MyTime::FindDate(string &_date)
{
#ifdef _WIN32
  const int kBufferLen = 5, kBase = 10;
#else
  const int kBufferLen = 5, kBase = 10;
#endif
  char buffer[kBufferLen];
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",this->day);
  if (this->day < kBase){
    _date += "0";
    _date += buffer;
  } else {
      _date += buffer;
  }
  _date += "/";
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",this->month);
  if (this->month < kBase){
    _date += "0";
    _date += buffer;
  } else {
      _date += buffer;
  }
  _date += "/";
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",this->year);
  _date += buffer;
  _date += "] ";
}

void MyTime::FindTime(string &_time)
{
#ifdef _WIN32
  int const kBufferLen = 2, kBase = 10;
#else
  int const kBufferLen = 3, kBase = 10;
#endif
  char buffer[kBufferLen];
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",this->hour);
  _time += "[";
  if (this->hour < kBase){
    _time += "0";
    _time += buffer;
  } else {
      _time += buffer;
  }
  _time += ":";
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",this->minute);
  if (this->minute < kBase) {
    _time += "0";
    _time += buffer;
  } else {
      _time += buffer;
  }
  _time += ":";
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",this->second);
  if (this->second < kBase) {
    _time += "0";
    _time += buffer;
  } else {
      _time += buffer;
  }
}
