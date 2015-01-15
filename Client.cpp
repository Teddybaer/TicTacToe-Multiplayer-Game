#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

void  DisplaySpielbrett(const char* Spielbrett)
{
	system("cls");
	for (int i = 0; i != 9; ++i)
	{
		if (i % 3 == 0 && i != 0)
		{
			std::cout << std::endl;
		}
		std::cout << Spielbrett[i] << " ";
	}
}

bool Spielfeldbesetzt(const char* spielbrett, int value)
{
	if (value < 0 || value > 8)
		return true;
	if (spielbrett[value] == 'X' || spielbrett[value] == 'O')
		return true;
	else
		return false;
}

void Spielfeldwahl(char* spielbrett, char spieler)
{
	int auswahl;

	do{
		system("cls");
		DisplaySpielbrett(spielbrett);
		std::cout << std::endl << "Waehlen sie ihr Spielfeld aus:";
		std::cin >> auswahl;
		--auswahl;
		std::cin.sync(); std::cin.clear();

	} while (Spielfeldbesetzt(spielbrett, auswahl));

	spielbrett[auswahl] = spieler;
}


char GewinnerCheck(const char* spielbrett)
{
	char Typen[2] = { 'X', 'O' };
	for (int i = 0; i != 2; ++i)
	{
		if (spielbrett[0] == Typen[i] && spielbrett[1] == Typen[i] && spielbrett[2] == Typen[i] ||
			spielbrett[3] == Typen[i] && spielbrett[4] == Typen[i] && spielbrett[5] == Typen[i] ||
			spielbrett[6] == Typen[i] && spielbrett[7] == Typen[i] && spielbrett[8] == Typen[i] ||

			spielbrett[0] == Typen[i] && spielbrett[3] == Typen[i] && spielbrett[6] == Typen[i] ||
			spielbrett[1] == Typen[i] && spielbrett[4] == Typen[i] && spielbrett[7] == Typen[i] ||
			spielbrett[2] == Typen[i] && spielbrett[5] == Typen[i] && spielbrett[8] == Typen[i] ||

			spielbrett[0] == Typen[i] && spielbrett[4] == Typen[i] && spielbrett[8] == Typen[i] ||
			spielbrett[6] == Typen[i] && spielbrett[4] == Typen[i] && spielbrett[2] == Typen[i]
			)
			return Typen[i];
	}

	return ' ';
}

int StartWs()
{
	WSAData wsa;
	return WSAStartup(MAKEWORD(2, 0), &wsa);
}

int main()
{
	char gegnerName[30];
	char meinName[30];
	std::cout << "Geben Sie ihren Namen ein:";
	std::gets(meinName);

	SOCKET sConnection;
	SOCKADDR_IN sinType;
	int nFehlercode = StartWs();
	if (nFehlercode != 0)
	{
		std::cout << "Winsocket startup error" << std::endl;
		std::cout << "Getlasterror:" << GetLastError() << std::endl;
		return 1;
	}

	sConnection = socket(AF_INET, SOCK_STREAM, 0);

	if (sConnection == INVALID_SOCKET)
	{
		std::cout << "Socket wurde nicht erstellt... " << std::endl;
		std::cout << "GetLasterror:" << GetLastError() << std::endl;
		return 1;
	}

	memset(&sinType, 0, sizeof(sinType));
	sinType.sin_addr.s_addr = inet_addr("127.0.0.1");
	sinType.sin_family = AF_INET;
	sinType.sin_port = htons(12345);

	nFehlercode = connect(sConnection, (SOCKADDR*)&sinType, sizeof(sinType));

	if (nFehlercode == SOCKET_ERROR)
	{
		std::cout << "Verbindung zum Server wurde nicht hergestellt" << std::endl;
		return 1;
	}

	int  Spielerstart;
	char Spielertyp;

	std::cout << "Auf Spieler Typ warten" << std::endl;

	nFehlercode = recv(sConnection, &Spielertyp, 1, 0);

	if (nFehlercode == SOCKET_ERROR || nFehlercode == 0)
	{
		std::cout << "SpielerTyp nicht empfangen..." << std::endl;
		return 1;
	}
	std::cout << "Spieler Typ erhalten" << std::endl;

	if (Spielertyp == 'X')
		Spielerstart = 0;
	else
		Spielerstart = 1;


	std::cout << "Meinen Namen an Server senden" << std::endl;
	nFehlercode = send(sConnection, meinName, strlen(meinName), 0);
	if (nFehlercode == SOCKET_ERROR || nFehlercode == 0)
	{
		std::cout << "SpielerTyp nicht empfangen..." << std::endl;
		return 1;
	}

	std::cout << "Gegner namen an Server senden" << std::endl;
	nFehlercode = recv(sConnection, gegnerName, 30, 0);
	if (nFehlercode == SOCKET_ERROR || nFehlercode == 0)
	{
		std::cout << "SpielerTyp nicht empfangen..." << std::endl;
		return 1;
	}
	gegnerName[nFehlercode] = '\0';

	std::cout << "Gegnername: " << gegnerName << std::endl;
	std::cout << "Sie sind Spieler " << Spielertyp << std::endl;

	char Spielbrett[30] = {'1','2','3','4','5','6','7','8', '9' };
	int  counter = 0;
	char Gewinner;
	std::cout << "Spiel starten" << std::endl;

	DisplaySpielbrett(Spielbrett);


	while (counter < 9)
	{
		if (Spielerstart == 0 || Spielerstart % 2 == 0)
		{
			std::cout  << std::endl << "Sie sind am Zug..." << std::endl << std::endl;
			DisplaySpielbrett(Spielbrett);
			Spielfeldwahl(Spielbrett, Spielertyp);
			nFehlercode = send(sConnection, Spielbrett, strlen(Spielbrett), 0);

			if (nFehlercode == SOCKET_ERROR || nFehlercode == 0)
			{
				std::cout << "SpielerTyp nicht empfangen..." << std::endl;
				return 1;
			}

			DisplaySpielbrett(Spielbrett);

			std::cout << std::endl << "Sie haben ihren Zug gemacht..." << std::endl;
			Gewinner = GewinnerCheck(Spielbrett);
			if (Gewinner == Spielertyp)
			{
				std::cout<< std::endl << "Sie haben Gewonnen " << std::endl;
				std::cin.sync(); std::cin.clear();
				std::cin.get();
				return 1;
			}
			else if (Gewinner != Spielertyp && Gewinner != ' ')
			{
				std::cout << std::endl << gegnerName << " hat gewonnen" << std::endl;
				std::cin.sync(); std::cin.clear();
				std::cin.get();
				return 1;
			}

		}
		else if (Spielerstart == 1 || Spielerstart % 2 != 0)
		{
			std::cout<< std::endl << "Ihr Gegner " << gegnerName << " ist am Zug..." << std::endl;
			nFehlercode = recv(sConnection, Spielbrett, 30, 0);
			Spielbrett[nFehlercode] = '\0';
			std::cout << std::endl << "Ihr Gegener hat seinen zug gemacht..." << std::endl;
			DisplaySpielbrett(Spielbrett);
			Gewinner = GewinnerCheck(Spielbrett);

			if (Gewinner == Spielertyp)
			{
				std::cout << std::endl << "Sie haben Gewonnen " << std::endl;
				std::cin.sync(); std::cin.clear();
				std::cin.get();
				return 1;
			}
			else if (Gewinner != Spielertyp && Gewinner != ' ')
			{
				std::cout << std::endl << gegnerName << " hat das Spiel gewonnen" << std::endl;
				std::cin.sync(); std::cin.clear();
				std::cin.get();
				return 1;
			}

		}

		++Spielerstart;
		++counter;
	}

	std::cout << "Es steht unentschieden..." << std::endl;
	std::cin.sync(); std::cin.clear();
	std::cin.get();

	return 0;
}
