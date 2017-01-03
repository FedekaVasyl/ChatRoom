#include "tcp_socket.h"
#include "error.h"

TCPSocket::TCPSocket() : kPort(9999)
{
  this->ip_address.clear();
  this->socket_descriptor = INVALID_SOCKET;
  this->Update();
  this->timer = new QTimer();
  const int kInterval = 500;
  this->timer->setInterval(kInterval);
}

TCPSocket::TCPSocket(int _socket, sockaddr_in *_address) : kPort(9999)
{
  /*IP address:   xxx.xxx.xxx.xxx = 15*/
  const int kIPLen = 15;
  char ip[kIPLen];
  this->socket_descriptor = _socket;
  strcpy(ip,inet_ntoa((struct in_addr)_address->sin_addr));
  this->ip_address = ip;
  this->Update();
  this->timer = new QTimer();
  const int kInterval = 500;
  this->timer->setInterval(kInterval);
}

TCPSocket::~TCPSocket()
{
  delete [] this->lan_addr;
  delete [] this->lan_name;
}

string TCPSocket::GetIP() const
{
  return this->ip_address;
}

const int TCPSocket::GetPort() const
{
  return this->kPort;
}

int TCPSocket::GetSocket() const
{
  return this->socket_descriptor;
}

int TCPSocket::GetLanCount() const
{
  return this->lan_count;
}

string* TCPSocket::GetLanAddr() const
{
  return this->lan_addr;
}

string TCPSocket::GetLanAddr(int _position) const
{
  return this->lan_addr[_position];
}

string* TCPSocket::GetLanName() const
{
  return this->lan_name;
}

string TCPSocket::GetLanName(int _position) const
{
  return this->lan_name[_position];
}

void TCPSocket::SetIP(string _ip)
{
  this->ip_address = _ip;
}

void TCPSocket::SetSocket(int _socket)
{
  this->socket_descriptor = _socket;
}

void TCPSocket::SetLanCount(int _count)
{
  this->lan_count = _count;
}

QTimer* TCPSocket::GetTimer() const
{
  return this->timer;
}

#ifdef _WIN32
  HANDLE TCPSocket::GetFindingHandle() const
  {
    return this->finding_handle;
  }

  void TCPSocket::SetFindingHandle(HANDLE _handle)
  {
    this->finding_handle = _handle;
  }
#else
  pthread_t TCPSocket::GetFindingHandle() const
  {
    return this->finding_handle;
  }

  void TCPSocket::SetFindingHandle(pthread_t _handle)
  {
    this->finding_handle = _handle;
  }
#endif

void TCPSocket::SetLanAddr(string _addr, int _position)
{
  this->lan_addr[_position] = _addr;
}

void TCPSocket::CreateLanAddr()
{
  this->lan_addr = new string[this->lan_count];
  for(int i = 0; i < this->lan_count; i++) {
    this->lan_addr[i].clear();
  }
}

void TCPSocket::SetLanName(string _name, int _position)
{
  this->lan_name[_position] = _name;
}

void TCPSocket::SetNameAndAddr(string _name, string _addr)
{
  for(int i = 0; i < this->lan_count; i++) {
    if(this->lan_name[i].empty()) {
      this->SetLanAddr(_addr,i);
      this->SetLanName(_name,i);
      break;
    }
  }
}

void TCPSocket::CreateLanName()
{
  this->lan_name = new string[this->lan_count];
  for(int i = 0; i < this->lan_count; i++) {
    this->lan_name[i].clear();
  }
}

int TCPSocket::FindLanName(string _name)
{
  for(int i = 0; i < this->lan_count; i++) {
    if(this->lan_name[i] == _name) {
      return i;
    }
  }
  return NO_LAN_NAME;
}

void TCPSocket::Update()
{
  this->lan_count = 1;
  this->CreateLanAddr();
  this->lan_addr[0].clear();
  this->CreateLanName();
  this->lan_name[0].clear();
}



#ifdef _WIN32
bool TCPSocket::SockInit()
{
  WSADATA wsaData;
  int result = 0;
  /* Initialize Winsock */
  result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    Error *error = new Error((int) WSAGetLastError());
    error->SetMessage("WSAStartup() failed");
    emit error->Show();
    delete error;
    return false;
  }
  return true;
}
#endif

void TCPSocket::SockExit()
{
  int result = 0;
#ifdef _WIN32
  result = closesocket(this->socket_descriptor);
  if (result == SOCKET_ERROR) {
    Error *error = new Error((int) WSAGetLastError());
    error->SetMessage("closesocket() failed");
    emit error->Show();
    delete error;
    return;
  }
  result = WSACleanup();
  if (result) {
    Error *error = new Error((int) WSAGetLastError());
    error->SetMessage("WSACleanup() failed");
    emit error->Show();
    delete error;
    return;
  }
#else
  result = shutdown(this->socket_descriptor,2);
  if (result == SOCKET_ERROR) {
    Error *error = new Error();
    error->SetCode((int) errno);
    error->SetMessage("shutdown() failed");
    emit error->Show();
    delete error;
    return;
  }
  SetParam();
#endif
}


bool TCPSocket::SetParam()
{
  bool opt_value = false;
  socklen_t size_opt_value = sizeof(socklen_t);
  int result = 0;
  result = setsockopt(this->socket_descriptor,SOL_SOCKET,\
                      SO_REUSEADDR,(char*)&opt_value,size_opt_value);
  if (result == SOCKET_ERROR) {
    Error *error = new Error();
    error->SetCode((int) errno);
    error->SetMessage("setsockopt() failed");
    emit error->Show();
    delete error;
    return false;
  }
#ifndef _WIN32
  result = setsockopt(this->socket_descriptor,SOL_SOCKET,\
                      SO_REUSEPORT,(char*)&opt_value,size_opt_value);
  if(result == SOCKET_ERROR) {
    Error *error = new Error();
    error->SetCode((int) errno);
    error->SetMessage("setsockopt() failed");
    emit error->Show();
    delete error;
    return false;
  }
#endif
  return true;
}


void TCPSocket::Send(Message* _message)
{
  int start = 0, end = MAX_MESSAGE_LEN - 1;
  int count_package_send = 0;
  string sub_str;
  sub_str.clear();
  int ssss = 0;
  if(end >= _message->GetMessageLen()) {
    end = _message->GetMessageLen();
  }
  sub_str = _message->GetMyMessage().substr(start,end);
  count_package_send = int(sub_str.size() / MAX_MESSAGE_LEN) + 1;
  /*send(this->GetSocket(),std::to_string(count_package_send).c_str(),\
       std::to_string(count_package_send).size(),0);*/
  do {
    send(this->GetSocket(),sub_str.c_str(),\
         MAX_MESSAGE_LEN,0);
    start = end;
    end += MAX_MESSAGE_LEN;
    if(end >= _message->GetMessageLen()) {
      end = _message->GetMessageLen();
    }
    sub_str.clear();
    if(start == end){
      break;
    }
    sub_str = _message->GetMyMessage().substr(start,end);
    count_package_send--;
  } while(count_package_send > 0);

}

int TCPSocket::Receive(char* receive_buf, \
                        Message* _message, \
                        int count_package_recv)
{
  int byte_count = 0;
  while(count_package_recv) {
    byte_count = recv(this->GetSocket(),receive_buf,\
                      MAX_MESSAGE_LEN,0);
    /*byte_count = WSAGetLastError();
    if(byte_count != 0){
      int tttt;
      tttt = 0;
    }*/
    _message->AddToMessage(receive_buf);
    memset(receive_buf,0,MAX_MESSAGE_LEN);
    count_package_recv--;
  }
  return byte_count;
}

#ifdef _WIN32
string TCPSocket::DetermineIP(bool is_current, char* computer_name)
{
  /*IP_ADAPTER_INFO *adapter_info;
  ULONG buf_len = sizeof(IP_ADAPTER_INFO);
  DWORD result;
  string ip;
  adapter_info = (IP_ADAPTER_INFO*)GlobalAlloc(GPTR,sizeof(IP_ADAPTER_INFO));
  if(ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(adapter_info, &buf_len)) {
    GlobalFree(adapter_info);
    adapter_info = (IP_ADAPTER_INFO*)GlobalAlloc(GPTR,buf_len);
  }
  if (result = GetAdaptersInfo(adapter_info,&buf_len)) {
    GlobalFree(adapter_info);
    Error *error = new Error((int) WSAGetLastError());
    error->SetMessage("GetAdaptersInfo() failed!");
    emit error->Show();
    delete error;
    return;
  }*/
  string ip;
  const int kComputerNameLen = 256;
  DWORD size = 256;
  int i = 0;
  //char computer_name[kComputerNameLen];
  struct hostent *remote_host;
  struct in_addr addr;
  //memset(computer_name,0,kComputerNameLen);
  this->SockInit();
  ip.clear();
  if(is_current) {
    computer_name = new char[kComputerNameLen];
    memset(computer_name,0,kComputerNameLen);
    GetComputerNameA(computer_name,&size);
  }

  remote_host = gethostbyname(computer_name);

  while (remote_host->h_addr_list[i] != 0) {
    addr.s_addr = *(u_long *) remote_host->h_addr_list[i++];
    ip = inet_ntoa(addr);
  }


  //ip = adapter_info->IpAddressList.IpAddress.String;
  if(is_current) {
    this->SetIP(ip);
    delete computer_name;
    return "1";
  }
  else {
    return ip;
  }
}
#else
string TCPSocket::DetermineIP(bool is_current, char* computer_name)
{
  struct ifaddrs* addr = NULL;
  struct ifaddrs* ifa = NULL;
  void *tmp_addr = NULL;
  char address_buffer[INET_ADDRSTRLEN];
  memset(address_buffer,0,INET_ADDRSTRLEN);
  getifaddrs(&addr);
  for(ifa = addr; ifa != NULL; ifa = ifa->ifa_next) {
    if(!ifa->ifa_addr) {
      continue;
    }
    if(ifa->ifa_addr->sa_family == AF_INET) {
      // is a valid IP4 Address
      tmp_addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
      inet_ntop(AF_INET, tmp_addr, address_buffer, INET_ADDRSTRLEN);
      }
  }
  this->SetIP(address_buffer);
  if(addr!=NULL) {
    freeifaddrs(addr);
  }
  return std::to_string(1);
}

#endif

#ifdef _WIN32
  void TCPSocket::FindComputers()
  {
    LPSERVER_INFO_101 pBuf = NULL;
    LPSERVER_INFO_101 pTmpBuf;
    DWORD dwLevel = 101;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 1;
    DWORD dwTotalCount = 0;
    DWORD dwServerType = SV_TYPE_SERVER;
    DWORD dwResumeHandle = 0;
    NET_API_STATUS nStatus;
    LPWSTR pszServerName = NULL;
    LPWSTR pszDomainName = NULL;
    DWORD i;
    const int kComputerNameLen = 256;
    char computer_name[kComputerNameLen];
    LPWSTR str;
    //while(APPLICATION_RUNNING) {
      memset(computer_name,0,kComputerNameLen);
      nStatus = NetServerEnum(pszServerName,
                              dwLevel,
                              (LPBYTE *) & pBuf,
                              dwPrefMaxLen,
                              &dwEntriesRead,
                              &dwTotalEntries,
                              dwServerType,
                              pszDomainName,
                              &dwResumeHandle);

      if((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {
          if((pTmpBuf = pBuf) != NULL) {
            this->SetLanCount(int(dwEntriesRead));
            if(this->GetLanCount() != 0) {
              this->CreateLanAddr();
              this->CreateLanName();
            }
            for(i = 0; i < dwEntriesRead; i++) {
              str = pTmpBuf->sv101_name;
              for(int j = 0; j < kComputerNameLen; j++) {
                memcpy(computer_name + j,str + j,1);
              }
              string ip = this->DetermineIP(false,computer_name);
              this->SetLanAddr(ip,i);
              pTmpBuf++;
              dwTotalCount++;
              memset(computer_name,0,kComputerNameLen);
            }
         }
        emit this->ShowAvailableChat();
      }
      if(pBuf != NULL) {
        NetApiBufferFree(pBuf);
      }
    //}
      //emit timer->timeout(-1);
      //this->GetTimer()->setInterval(100);
  }
#else
  void TCPSocket::FindComputers()
  {
    system("arp -a");
    FILE *arpCache = fopen(ARP_CACHE, "r");
    char header[ARP_BUFFER_LEN], device[ARP_BUFFER_LEN];
    char ip_addr[ARP_BUFFER_LEN], hw_addr[ARP_BUFFER_LEN];
    int i = 0;
    memset(header,0,ARP_BUFFER_LEN);
    memset(ip_addr,0,ARP_BUFFER_LEN);
    memset(hw_addr,0,ARP_BUFFER_LEN);
    memset(device,0,ARP_BUFFER_LEN);
    if(!arpCache) {
      Error *error = new Error();
      error->SetCode((int) errno);
      error->SetMessage("Arp Cache: Failed to open file \"" ARP_CACHE "\"");
      emit error->Show();
      delete error;
      return;
    }
    /*Ignore the first line, which contains the header*/

    if(!fgets(header, sizeof(header), arpCache)) {
      return;
    }
    this->SetLanCount(1000);
    this->CreateLanAddr();
    this->CreateLanName();
    while(fscanf(arpCache, ARP_LINE_FORMAT, ip_addr, hw_addr, device) == 3) {
      this->SetLanAddr(ip_addr,i);
      i++;
    }
    //fclose(arpCache);
    //timer->stop();
    /*struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
      if(!ifa->ifa_addr) {
        continue;
      }
      if(ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
        // is a valid IP4 Address
        tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        char addressBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
        printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
      }
      else if(ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
        // is a valid IP6 Address
        tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
        char addressBuffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
        printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
      }
    }
    if(ifAddrStruct!=NULL) {
      freeifaddrs(ifAddrStruct);
    }*/
  }
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
