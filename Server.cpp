#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>


int StartWs()
{
	WSAData wsa;
	return WSAStartup(MAKEWORD(2, 0), &wsa);
}

#define MAX 2

int fullClientsArray(SOCKET* sarr)
{

	int zaehler = 0;
	for (int i = 0; i != MAX; ++i)
	{
		if (sarr[i] != INVALID_SOCKET)
			++zaehler;
	}

	return zaehler;
}

int main()
{
	SOCKET sConnection;
	SOCKET clients[MAX];
	FD_SET fdSet;
	SOCKADDR_IN sinType;

	int nFehlercode = StartWs();
	sConnection = socket(AF_INET, SOCK_STREAM, 0);
	
	for (int i = 0; i != MAX; ++i)
		clients[i] = INVALID_SOCKET;

	memset(&sinType, 0, sizeof(sinType));
	sinType.sin_addr.s_addr = INADDR_ANY;
	sinType.sin_family = AF_INET;
	sinType.sin_port = htons(12345);


	nFehlercode = bind(sConnection, (SOCKADDR*)&sinType, sizeof(sinType));
	nFehlercode = listen(sConnection, MAX);


	while (fullClientsArray(clients) != MAX)
	{ 
		FD_ZERO(&fdSet);
		FD_SET(sConnection, &fdSet);

		nFehlercode = select(0, &fdSet, nullptr, nullptr, nullptr);
		if (FD_ISSET(sConnection, &fdSet))
			for (int i = 0; i != MAX; ++i)
				if (clients[i] == INVALID_SOCKET)
					clients[i] = accept(sConnection, nullptr, nullptr);
	}

	std::cout << " 2 Verbindungen wurden angenommen " << std::endl;
	char GamePlayerTyp[2] = { 'X', 'O' };

	for (int i = 0; i != MAX; ++i)
		nFehlercode = send(clients[i], &GamePlayerTyp[i], 1, 0);


	char client0username[30];
	char client1username[30];

	for (int i = 0; i != MAX; ++i)
	{
		if (i == 0)
		{
			nFehlercode = recv(clients[0], client0username, 30, 0);
			client0username[nFehlercode] = '\0';
		}
		else if (i == 1)
		{
			nFehlercode = recv(clients[1], client1username, 30, 0);
			client1username[nFehlercode] = '\0';
		}

	}

	for (int i = 0; i != MAX; ++i)
	{
		if (i == 0)
	        	nFehlercode = send(clients[0], client1username, strlen(client1username), 0);
		else if (i == 1)
			nFehlercode = send(clients[1], client0username, strlen(client0username), 0);
	}


	int zaehler = 0;
	char Spielbrett[30];

	while (true)
	{
		FD_ZERO(&fdSet);
		FD_SET(clients[0], &fdSet);
		FD_SET(clients[1], &fdSet);
		nFehlercode = select(0, &fdSet, nullptr, nullptr, nullptr);

		for (int i = 0; i != MAX; ++i)
		{
			if (FD_ISSET(clients[i], &fdSet))
			{
				if (i == 0)
				{
					nFehlercode = recv(clients[0], Spielbrett, 30, 0);
					Spielbrett[nFehlercode] = '\0';
					nFehlercode = send(clients[1], Spielbrett, strlen(Spielbrett), 0);
				}
				else
				{

					nFehlercode = recv(clients[1], Spielbrett, 30, 0);
					Spielbrett[nFehlercode] = '\0';
					nFehlercode = send(clients[0], Spielbrett, strlen(Spielbrett), 0);
				}

			}
		}



	}

	return 0;
}
