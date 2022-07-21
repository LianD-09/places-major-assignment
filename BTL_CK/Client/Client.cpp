// TCPClient.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

#define BUFF_SIZE 1024
#define af AF_INET
#define SERVERIP "127.0.0.1"

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	WORD vVersion = MAKEWORD(2, 2);
	if (WSAStartup(vVersion, &wsaData))
		cout << "Version if not supported! \n";
	SOCKET client = socket(af, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in serverAddr;
	serverAddr.sin_family = af;
	serverAddr.sin_port = htons(5500);
	inet_pton(af, SERVERIP, &serverAddr.sin_addr);

	if (connect(client, (sockaddr *)&serverAddr, sizeof serverAddr)) {
		cout << "Cannot connect. Error: " << WSAGetLastError() << "\n";
		return 0;
	}

	char buff[BUFF_SIZE];
	char rbuff[BUFF_SIZE];
	int ret, messageLen;

	while (1) {
		cout << "Sent to server: ";
		gets_s(buff, BUFF_SIZE);
		strcat_s(buff, "\r\n");
		messageLen = strlen(buff);

		if (messageLen == 0) break;

		ret = send(client, buff, messageLen, 0);
		memset(rbuff, 0, sizeof(rbuff));
		ret = recv(client, rbuff, BUFF_SIZE, 0);
		cout << rbuff;

	}

	closesocket(client);


	WSACleanup();
}