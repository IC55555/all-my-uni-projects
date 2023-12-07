#pragma comment (lib,"Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS_

#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <iostream> 
#include <clocale>
using namespace std;
int main()
{
   SetConsoleCP(1251);
   SetConsoleOutputCP(1251);
   //setlocale(LC_ALL, "ru_RU.UTF-16");
   WORD ver = MAKEWORD(2, 2);
   WSADATA wsaData;
   int retVal = 0;
   WSAStartup(ver, (LPWSADATA)&wsaData);
   LPHOSTENT hostEnt;
   hostEnt = gethostbyname("localhost");
   if (!hostEnt)
   {
      printf("Unable to collect gethostbyname\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   //—оздаем сокет
   SOCKET clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (clientSock == SOCKET_ERROR)
   {
      printf("Unable to create socket\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   string ip;
   cout << "Enter servers's ip>";
   cin >> ip;
   cin.ignore();

   SOCKADDR_IN serverInfo;   serverInfo.sin_family = PF_INET;
   serverInfo.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
   serverInfo.sin_port = htons(2007);
   //ѕытаемс€ присоединитс€ к серверу по ip и port
   retVal = connect(clientSock, (LPSOCKADDR)&serverInfo, sizeof(serverInfo));
   if (retVal == SOCKET_ERROR)
   {
      printf("Unable to connect\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   printf("Connection made sucessfully\n");
   printf("Enter directory or 'stop' to shutdown server\n");
   char pBuf[256];
   gets_s(pBuf); //отправл€етс€ на сервер
   //cout << pBuf << endl;
   printf("Sending request from client\n");
   //ќтсылаем данные на сервер
   retVal = send(clientSock, pBuf, strlen(pBuf) + 1, 0);
   if (retVal == SOCKET_ERROR)
   {
      printf("Unable to send\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   char szResponse[256];
   //ѕытаемс€ получить ответ от сервера
   retVal = recv(clientSock, szResponse, 256, 0);
   if (retVal == SOCKET_ERROR)
   {
      printf("Unable to recv\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   char *Resp;
   Resp = szResponse;
   if (pBuf[0] != 's')
      printf("Result: %s\n", Resp);
   else
      printf("%s\n", Resp);
   closesocket(clientSock);
   WSACleanup();
   system("pause");
   return 0;
}
//не в водить слова на s;