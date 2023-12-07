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

void demo_status(const fs::path &p, fs::file_status s)
{
	std::cout << p;
	//alternative: switch(s.type()) { case fs::file_type::regular: ...}
	if (fs::is_regular_file(s)) std::cout << " is a regular file\n";
	if (fs::is_directory(s)) std::cout << " is a directory\n";
	if (fs::is_block_file(s)) std::cout << " is a block device\n";
	if (fs::is_character_file(s)) std::cout << " is a character device\n";
	if (fs::is_fifo(s)) std::cout << " is a named IPC pipe\n";
	if (fs::is_socket(s)) std::cout << " is a named IPC socket\n";
	if (fs::is_symlink(s)) std::cout << " is a symlink\n";
	if (!fs::exists(s)) std::cout << " does not exist\n";
}


int main()
{
	setlocale(LC_ALL, "Rus");
	WORD sockVer;
	WSADATA wsaData;
	int retVal;
	sockVer = MAKEWORD(2, 2);
	WSAStartup(sockVer, &wsaData);
	//Создаем сокет
	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (servSock == INVALID_SOCKET)
	{
		printf("Unable to create socket\n");
		WSACleanup();
		system("pause");
		return SOCKET_ERROR;
	}

	SOCKADDR_IN sin;
	sin.sin_family = PF_INET;
	sin.sin_port = htons(2007);
	sin.sin_addr.s_addr = inet_addr("25.18.182.36");

	retVal = bind(servSock, (LPSOCKADDR)&sin, sizeof(sin));
	if (retVal == SOCKET_ERROR)
	{
		printf("Unable to bind\n");
		WSACleanup();
		system("pause");
		return SOCKET_ERROR;
	}
	printf("Server started at %s, port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
	while (true)
	{
		//Пытаемся начать слушать сокет
		retVal = listen(servSock, 10);
		if (retVal == SOCKET_ERROR)
		{
			printf("Unable to listen\n");
			WSACleanup();
			system("pause");
			return SOCKET_ERROR;
		}
		//Ждем клиента
		SOCKET clientSock;
		SOCKADDR_IN from;
		int fromlen = sizeof(from);
		clientSock = accept(servSock, (struct sockaddr *)&from, &fromlen);
		if (clientSock == INVALID_SOCKET)
		{
			printf("Unable to accept\n");
			WSACleanup();
			system("pause");
			return SOCKET_ERROR;
		}
		printf("New connection accepted from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port));
		char buff[256];
		retVal = recv(clientSock, buff, 256, 0);
		//Пытаемся получить данные от клиента
		if (retVal == SOCKET_ERROR)
		{
			printf("Unable to recv\n");
			system("pause");
			return SOCKET_ERROR;
		}
		printf("Data received\n");
		string s = (const char *)buff;
		if (s[0] == 's')// Команда на выключение сервера
		{
			char szResp[256] = "Server shutdown";
			retVal = send(clientSock, szResp, 256, 0);

			closesocket(clientSock);
			break;
		}
		else
		{
			
			//SOCKET *client_socket = new SOCKET[1]();
			string corner = "->";
			SOCKET my_sock = servSock;
			char buff[256];
			#define sHELLO "Hello\r\n"
			// приветствие клиента
			int bytes_recv;
			/*cout << s << endl;
			bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0);
			string dir_Name = (const char *)buff;
			dir_Name.push_back('\n');*/
			//while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR)
			{
				//string dirName = (const char *)buff;
				string dirName = s;
				//dirName.resize(bytes_recv);
				
				size_t x = dirName.find("\\");

				/*while (x != string::npos)
				{
					dirName.replace(x, 1, "/");
					x = dirName.find("\\", x + 1);
				}*/

				cout << dirName << endl;
			fs::path Path = dirName;
			ostringstream stream;
			//dirName = "C:/тест";
			//fs::create_directory(dirName);
			demo_status(dirName, fs::status(dirName));
			
				if (fs::is_directory(Path))
				{
					uint16_t _allTab = std::count(dirName.begin(), dirName.end(), '\\');
					stream << Path << endl;
					for (auto &dirEntry : recursive_directory_iterator(Path))
					{
						dirName = dirEntry.path().string();
						uint16_t _Tab = std::count(dirName.begin(), dirName.end(), '\\');
						for (size_t _Pos = _allTab; _Pos < _Tab; _Pos++)
							stream << '\t';
						stream << corner << dirEntry.path().filename() << endl;
						cout << corner << dirEntry.path().filename() << endl;
						//bytes_recv = dirEntry.path().filename().string().size() + 1;
						//retVal = send(clientSock, (const char*)(dirEntry.path().filename().string().c_str()), bytes_recv, 0);
					}
					bytes_recv = stream.str().size() + 1;
					retVal = send(clientSock, (char *)stream.str().c_str(), bytes_recv, 0);
				}
				else
				{
					stream << "Non-existing directory" << endl;
					bytes_recv = stream.str().size() + 1;
					retVal = send(clientSock, (char *)stream.str().c_str(), bytes_recv, 0);
				}
					
			}
			/*nclients--;
			printf("-disconect\n");*/
		}
	}
	//Закрываем сокет	
	closesocket(servSock);
	WSACleanup();
	return 0;
}
