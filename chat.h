#include "server.h"
#include "client.h"
#include "chat_form.h"
#include "change_nickname_dialog.h"
#include "error.h"
//#include "\sdk\inc\iphlpapi.h"
//#include <iphlpapi.h>
#include <stdlib.h>
#include <vector>
//#include <WS2tcpip.h>
#include <iterator>
#include <QApplication>

#define MAX_CHAT_NAME_LEN 30
#ifndef CHAT_H
#define CHAT_H

#pragma comment(lib, "IPHLPAPI.lib")


class Chat : public Server
{
  Q_OBJECT
signals:
  void ShowChatForm();
  void Logging(string _str);
  void Saying(string _str);
  void Changing(string old_nickname, string new_nickname);
  void Disconnecting(string nickname, string message);
  void SetAvailable(string _name);
private:
#ifdef _WIN32
  HANDLE receiving_handle;
#else
  pthread_t receiving_handle;
#endif
  int client_count;
  string chat_name;
  Client **clients;
  Client *current_client;
  ChatForm *chat_form;
  ChangeNicknameDialog *change_dialog;
public slots:
  void ChangeNicknameClick();
  void ChangeNickname();
  void LogInToChat();
  void SayClient();
  void Disconnect();
  void TextChanged();
  void TextNicknameChanged();
  void TextSayChanged();
public:
  Chat();
  Chat(string _chat_name);
  ~Chat();
  int FindNickname(string _nickname);
  int GetClientCount() const;
  string GetChatName() const;
  void SetClientCount(int _count);
  void SetClientNickname(string _nickanme, int _position);
  void SetChatName(string _chat_name);
  Client** GetClients() const;
  void SetClient(Client *client, int client_id);
  Client* GetCurrentClient() const;
  void SetCurrentClient(Client *client);
  int FindClient(Client *client);   //Finding the client's position in the clients array.
  void Accepting();
  void Receiving(Client *client);
  void ProcessingUserRequests(int client_id);
  void Connecting(Message *_message);
#ifdef _WIN32
  HANDLE GetReceivingHandle() const;
  void SetReceivingHandle(HANDLE _handle);
  static UINT AcceptThreadFunction(LPVOID param)
#else
  pthread_t GetReceivingHandle() const;
  void SetReceivingHandle(pthread_t _handle);
  static void* AcceptThreadFunction(void* param)
#endif
    {
      Chat* This = (Chat*) param;
      This->Accepting();
#ifdef _WIN32
      return 1;
#else
      return (void*)1;
#endif
    }
  struct SetParamForReceive
  {
    Chat *chat;
    Client *client;
  } _receive;
#ifdef _WIN32
  static UINT ReceiveThreadFunction(LPVOID param)
#else
  static void* ReceiveThreadFunction(void* param)
#endif
    {
      Chat* This = ((Chat*)param)->_receive.chat;
      Client *client = ((Chat*)param)->_receive.client;
      This->Receiving(client);
#ifdef _WIN32
      return 1;
#else
      return (void*)1;
#endif
      /*Chat* This = (Chat*) param;
      This->Receiving();
      return 1;*/
    }
  struct ParamForThreadFunction
  {
    Chat *chat;
    int client_id;
  } _param;
#ifdef _WIN32
  static UINT ProcessingThreadFunction(LPVOID param)
#else
  static void* ProcessingThreadFunction(void* param)
#endif
  {
    Chat* This = ((Chat*)param)->_param.chat;
    int client_id = ((Chat*)param)->_param.client_id;
    This->ProcessingUserRequests(client_id);
#ifdef _WIN32
      return 1;
#else
      return (void*)1;
#endif
  }
#ifdef _WIN32
  static UINT FindingThreadFunction(LPVOID param)
#else
  static void* FindingThreadFunction(void* param)
#endif
    {
      Chat* This = (Chat*) param;
      This->FindComputers();
#ifdef _WIN32
      return 1;
#else
      return (void*)1;
#endif
    }
  void StartFinding();
  void SendToAll(Message *message);
  void SendTo(Client *client, Message *_message);
  void TryChangeNickname();
  ChatForm* GetChatForm() const;
  ChangeNicknameDialog * GetDialog() const;
};

#endif // CHAT_H
