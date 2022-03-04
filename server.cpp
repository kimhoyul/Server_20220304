#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <iostream>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket; //ip와 port를 프로그램에서 사용하기 위해서 매칭
	ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerAddr;
	memset(&ServerAddr, 0, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = PF_INET;
	ServerAddr.sin_addr.s_addr = INADDR_ANY;
	ServerAddr.sin_port = htons(50000);

	bind(ServerSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));

	listen(ServerSocket, 0);

	while (1)
	{
		SOCKADDR_IN ClientAddr;
		memset(&ClientAddr, 0, sizeof(ClientAddr));
		int ClientAddrSize = sizeof(ClientAddr);

		SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &ClientAddrSize);

		string Packet;
		char Buffer;
		bool bCommandComplete = false;
		while (1)
		{
			int RecvLength = recv(ClientSocket, &Buffer, 1, 0);
			if (RecvLength > 0)
			{
				if (Buffer == '\n')
				{
					bCommandComplete = true;
					break;
				}

				Packet += Buffer;
			}
			else
			{
				cout << GetLastError() << endl;
				closesocket(ClientSocket);
				break;
			}
		}

		if (!bCommandComplete)
		{
			continue;
		}

		cout << "Packet : " +  Packet << endl;

		string Command = Packet.substr(0, 3);
		string Filename = Packet.substr(4, Packet.length() - 4);
		if (Command == "put")
		{
			FILE* fp = fopen(Filename.c_str(), "wb");
			while (1)
			{
				char Buffer[2049] = { 0, };
				int RecvLength = recv(ClientSocket, Buffer, 2048, 0);
				if (RecvLength > 0)
				{
					fwrite(Buffer, sizeof(char), RecvLength, fp);
				}
				else
				{
					cout << "Complete save file : " << Filename << endl;
					fclose(fp);
					closesocket(ClientSocket);
					break;
				}
			}
		}
		else if (Command == "get")
		{

		}
		else if (Command == "qut")
		{
			cout << "Server End" << endl;
			break;
		}
	}

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}