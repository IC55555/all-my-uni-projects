#pragma comment (lib,"Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS_

#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <iostream> 
#include <clocale>
using namespace std;

SOCKET Connect;
void RecvToClient()
{
   char *buff = new char[256];
   for (; ; Sleep(75))
   {
      memset(buff, 0, sizeof(buff));
      if (recv(Connect, buff, 256, NULL) && strlen(buff) > 0)
      {
         printf("Сообщение:                         %s", buff);
         printf("\n");
      }
   }
}

void getWord(char *mas)
{
   int c;
   for (int i = 0; (c = getchar()) != EOF && c != '\n'; ++i)
      mas[i] = c;
}

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
   //Создаем сокет
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

 //----------------------------------------------------
   char message[MAX_PATH];
   WSAData wsa;
   WORD Version = MAKEWORD(2, 1);
   WSAStartup(Version, &wsa);
   if (WSAStartup(Version, &wsa) != 0)
   {
      cout << "Can't load WSA library." << endl;
   }
   else
   {
      cout << "WSA library load complite" << endl;
   }

   Connect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

   if (Connect == SOCKET_ERROR)
   {
      cout << "Ошибка создания сокета" << endl;
   }
   else
   {
      cout << "Сокет создан" << endl;
   }

   SOCKADDR_IN Client;

   Client.sin_family = AF_INET;
   Client.sin_port = htons(2007);
   Client.sin_addr.s_addr = inet_addr(ip.c_str());

   while (1) // !!!
   {
      if (connect(Connect, (SOCKADDR *)&Client, sizeof(Client)))
      {
         cout << "Подключен к серверу" << endl;
         send(Connect, "Сообщение от клиента: Подключен клиент.", MAX_PATH, 0);
         recv(Connect, message, sizeof(message), 0);
         CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RecvToClient, NULL, NULL, NULL);
         break;
      }
   }

   cout << message << endl;
   ZeroMemory(message, sizeof(message));

   while (1)
   {
      //cout << "Введите сообщение серверу" << endl;
      getWord(message);
      send(Connect, message, MAX_PATH, 0);
      ZeroMemory(message, sizeof(message));
   }

   closesocket(Connect);
   WSACleanup();
   system("pause");

   return 0;
   //----------------------------------------------------------

   SOCKADDR_IN serverInfo;   serverInfo.sin_family = PF_INET;
   serverInfo.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
   serverInfo.sin_port = htons(2007);
   //Пытаемся присоединится к серверу по ip и port
   retVal = connect(clientSock, (LPSOCKADDR)&serverInfo, sizeof(serverInfo));
   if (retVal == SOCKET_ERROR)
   {
      printf("Unable to connect\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   printf("Connection made sucessfully\n");
   char pBuf[256];
   printf("Type message: ");
   gets_s(pBuf); //отправляется на сервер
   char *Resp;
   char szResponse[256];

   while (pBuf != "Disconnect" && pBuf != "disconnect")
   {
      //Пытаемся получить ответ от сервера
      retVal = recv(clientSock, szResponse, 256, 0);
      if (retVal == SOCKET_ERROR)
      {
         printf("Unable to recv\n");
         WSACleanup();
         system("pause");
         return 1;
      }
      
      Resp = szResponse;
      cout << "                                          " << Resp << endl;

      printf("Type message: ");
      gets_s(pBuf);
   }
   closesocket(clientSock);
   WSACleanup();
   system("pause");
   return 0;
}
