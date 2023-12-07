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
#include <vector>

using std::experimental::filesystem::recursive_directory_iterator;
namespace fs = std::experimental::filesystem;
using namespace std;

SOCKET *Connections;
int ClientCount = 0;
vector<player_coords> players;


struct player_coords 
{
	double x, y, z;

	player_coords()
	{
		x, y, z = 0;
	}

	void move(int key);



	~player_coords(){}

};




void getWord(char *mas)
{
	int c;
	for (int i = 0; (c = getchar()) != EOF && c != '\n'; ++i)
		mas[i] = c;
}



void new_player()
{
	player_coords temp;
	
	players.push_back(temp);


}

void player_coords::move(int key)
{
	
	switch (key)
	{
	case 0x57://W(movment up)
		y -= 5;
	case 0x53://S(movment down)
		y += 5;
	case 0x41://A(movment left)
		x -= 5;
	case 0x44://D(movment right)
		x += 5;
	}
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

int init()
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
			send(Connections[ClientCount], m_connect, strlen(m_connect) + 1, NULL);
			recv(Connect, message, sizeof(message), 0);
			ClientCount++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendMessageToClient, (LPVOID)(ClientCount - 1), NULL, NULL);
		}
	}
}