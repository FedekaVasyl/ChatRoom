#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
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



#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTextBrowser>
#include <QListView>
#include <QStringListModel>
#include <QStringList>
#include <QListIterator>
#include <QProgressBar>
#include <QKeyEvent>
#include "server.h"
#include "chat.h"
#include "chat_form.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
private:
  Ui::MainWindow *ui;
  Chat *chat;
  QPushButton *connect_button;
  QPushButton *create_button;
  QPushButton *update_button;
  QTextEdit *selected_chat;
  QTextEdit *create_chat;
  QListView *available_chat;
  QStringListModel *list_model;
  QProgressBar *update_progress_bar;
public slots:
  void CreateNewChat();
  void ConnectToChat();
  void ShowMainForm();
  //void ShowChatForm();
  void CreateEnable();
  void UpdateAvailable();
  void ConnectEnable(QModelIndex _index);
  void SetAvailable(string _name);
  void Logging(string _str);
  void Saying(string _str);
  void Changing(string old_nickname, string new_nickname);
  void Disconnecting(string nickname, string message);
  void ShowAvailableChat();
  void TimerOverFlow();
  void UpdateClicked();
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  std::string GetChatNameText() const;
  std::string GetSelectedChatText() const;
  Chat* GetChat() const;
  QPushButton* GetConnectButton() const;
  QPushButton* GetCreateButton() const;
  QPushButton* GetUpdateButton() const;
  QTextEdit* GetSelectedChat() const;
  QTextEdit* GetCreateChat() const;
  QListView* GetAvailableChat() const;
  QStringListModel* GetListModel() const;
  QProgressBar* GetUpdateProgressBar() const;
};

#endif // MAINWINDOW_H
