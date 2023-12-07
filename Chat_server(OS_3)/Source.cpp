#pragma comment (lib,"Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS_

#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream> 
#include <sstream>
#include <string>
#include <filesystem>
#include <experimental/filesystem>
#include <algorithm>
#include <locale>

using std::experimental::filesystem::recursive_directory_iterator;
namespace fs = std::experimental::filesystem;
using namespace std;

SOCKET *Connections;
int ClientCount = 0;

void getWord(char *mas)
{
	int c;
	for (int i = 0; (c = getchar()) != EOF && c != '\n'; ++i)
		mas[i] = c;
}

void SendMessageToClient(int ID)
{
	int flag = 0;
	char buffer[MAX_PATH];
	for (;; Sleep(75))
	{
		if (recv(Connections[ID], buffer, MAX_PATH, NULL))
		{
			//if (ClientCount > 1)
			{
				printf("Сообщение: %s", buffer);
				printf("\n");
				for (int i = 0; i < ClientCount; ++i)
				{
					flag = send(Connections[i], buffer, MAX_PATH, NULL);
					/*if (flag == SOCKET_ERROR)
					{
						ClientCount--;
					}*/
				}
			}
		}
		ZeroMemory(buffer, sizeof(buffer));
	}
}

int main()
{
	setlocale(LC_ALL, "Rus");
	char message[MAX_PATH];
	const char *m_connect = "Сообщение от сервера : Вы подключились к серверу.";
	WSAData wsa;
	WORD Version = MAKEWORD(2, 1);
	WSAStartup(Version, &wsa);
	Connections = (SOCKET *)calloc(64, sizeof(SOCKET));
	if (WSAStartup(Version, &wsa) != 0)
	{
		cout << "WSA library startup error." << endl;
	}
	else
	{
		cout << "WSA library startup complete." << endl;
	}
	SOCKET Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKET Connect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Listen == SOCKET_ERROR || Connect == SOCKET_ERROR)
	{
		cout << "Ошибка создания сокета" << endl;
	}
	else
	{
		cout << "Сокет создан" << endl;
	}
	SOCKADDR_IN Server;
	Server.sin_family = PF_INET;
	Server.sin_port = htons(2007);
	Server.sin_addr.s_addr = inet_addr("25.18.182.36");
	printf("Server started at %s, port %d\n", inet_ntoa(Server.sin_addr), htons(Server.sin_port));
	if (bind(Listen, (SOCKADDR *)&Server, sizeof(Server)) == SOCKET_ERROR)
	{
		cout << "Ошибка соединение" << endl;
	}
	else
	{
		cout << "Сервер активен" << endl;
	}
	listen(Listen, SOMAXCONN);
	SOCKADDR_IN from;
	int fromlen = sizeof(from);
	while (true)
	{
		if (Connect = accept(Listen, (struct sockaddr *)&from, &fromlen))
		{

			printf("New connection accepted from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port));
			Connections[ClientCount] = Connect;
			send(Connections[ClientCount], m_connect, strlen(m_connect)+1, NULL);
			recv(Connect, message, sizeof(message), 0);
			ClientCount++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendMessageToClient, (LPVOID)(ClientCount - 1), NULL, NULL);
		}
	}
}






//WORD sockVer;
	//WSADATA wsaData;
	//int retVal;
	//sockVer = MAKEWORD(2, 2);
	//WSAStartup(sockVer, &wsaData);
	////Создаем сокет
	//SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//if (servSock == INVALID_SOCKET)
	//{
	//	printf("Unable to create socket\n");
	//	WSACleanup();
	//	system("pause");
	//	return SOCKET_ERROR;
	//}

	//SOCKADDR_IN sin;
	//sin.sin_family = PF_INET;
	//sin.sin_port = htons(2007);
	//sin.sin_addr.s_addr = inet_addr("25.18.182.36");

	//retVal = bind(servSock, (LPSOCKADDR)&sin, sizeof(sin));
	//if (retVal == SOCKET_ERROR)
	//{
	//	printf("Unable to bind\n");
	//	WSACleanup();
	//	system("pause");
	//	return SOCKET_ERROR;
	//}
	//printf("Server started at %s, port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
	//while (true)
	//{
	//	//Пытаемся начать слушать сокет
	//	retVal = listen(servSock, 10);
	//	if (retVal == SOCKET_ERROR)
	//	{
	//		printf("Unable to listen\n");
	//		WSACleanup();
	//		system("pause");
	//		return SOCKET_ERROR;
	//	}
	//	//Ждем клиента
	//	SOCKET clientSock;
	//	SOCKADDR_IN from;
	//	int fromlen = sizeof(from);
	//	clientSock = accept(servSock, (struct sockaddr *)&from, &fromlen);
	//	if (clientSock == INVALID_SOCKET)
	//	{
	//		printf("Unable to accept\n");
	//		WSACleanup();
	//		system("pause");
	//		return SOCKET_ERROR;
	//	}
	//	printf("New connection accepted from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port));
	//	char buff[256];

	//	/*retVal = recv(clientSock, buff, 256, 0);*/

	//	while (1 == 1)
	//	{
	//		retVal = send(clientSock, "test", 256, 0);
	//	}
	//	
	//	/*for (int i = 0; i < 4; i++)
	//	{
	//		retVal = send(clientSock, "test", 256, 0);
	//	}*/
	//	//Пытаемся получить данные от клиента
	//	if (retVal == SOCKET_ERROR)
	//	{
	//		printf("Unable to recv\n");
	//		system("pause");
	//		return SOCKET_ERROR;
	//	}
	//	printf("Data received\n");
	//	string s = (const char *)buff;
	//	if (s[0] == 's')// Команда на выключение сервера
	//	{
	//		char szResp[256] = "Server shutdown";
	//		retVal = send(clientSock, szResp, 256, 0);

	//		closesocket(clientSock);
	//		break;
	//	}
	//	else
	//	{
	//		ostringstream stream;
	//		int bytes_recv;
	//		stream << s << endl;
	//		bytes_recv = stream.str().size() + 1;
	//		retVal = send(clientSock, (char *)stream.str().c_str(), bytes_recv, 0);
	//	}
	//}
	////Закрываем сокет	
	//closesocket(servSock);
	//WSACleanup();
	//return 0;