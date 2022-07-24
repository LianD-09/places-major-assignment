#include "stdafx.h"

#include "stdio.h"
#include "conio.h"
#include "string.h" 
#include "ws2tcpip.h"
#include "winsock2.h"
#include "process.h"
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#define MAX_CLIENT 4069
#define MAX_ACCOUNT 2000

#define ENDING_DELIMITER "\r\n"

using namespace std;

typedef struct Client {
	SOCKET s;
	char clientIP[INET_ADDRSTRLEN];
	int clientPort;
	char username[BUFF_SIZE];
	int isLogin;
}Client;



typedef struct Account {
	int id_account;
	char userName[BUFF_SIZE];
	char password[BUFF_SIZE];
	int status; // se loai bo sau
}Account;

typedef struct Category {
	int id_category;
	char name[BUFF_SIZE];
}Category;

typedef struct Place {
	int id_place;
	int id_category;
	char name[BUFF_SIZE];
}Place;

typedef struct Friend {
	int id_account1;
	int id_account2;
}Friend;

typedef struct Favorite {
	int id_account;
	int id_place;
};

typedef struct Notification {
	int tag;// 0 is Tag, 1 is add friend
	int id_account1;
	int id_account2;
	int id_place;
	int isSeen;//0 no seen, 1 seen chua xu ly, 2 seen va da xu ly 
};

vector<Account> arrAccount;
vector<Category> arrCategory;
vector<Place> arrPlace;
vector<Friend> arrFriend;
vector<Favorite> arrFavorite;
vector<Notification> arrNotification;


CRITICAL_SECTION critical;
int cnt = 0;
Client arrClient[MAX_CLIENT * 2];

char queueBuff[MAX_CLIENT][11 * BUFF_SIZE];
fd_set readfsd[10000], initfds[10000], writefds[10000];





void getHeader(char* message, char* header) {
	char buffHeader[BUFF_SIZE];
	memset(buffHeader, 0, sizeof(buffHeader));
	for (int i = 0; i < strlen(message) && message[i] != ' '; i++) {
		buffHeader[i] = message[i];
	}
	memcpy(header, buffHeader, BUFF_SIZE);

}


/* The recv() wrapper function */
int Receive(SOCKET s, char *buff, int size, int flags) {
	int n;

	n = recv(s, buff, size, flags);
	if (n == SOCKET_ERROR)
		printf("Error: %", WSAGetLastError());

	return n;
}

int getIdFromUsername(char* username) {
	for (int i = 0; i < arrAccount.size(); i++) {
		if (!strcmp(arrAccount[i].userName, username)) {
			return arrAccount[i].id_account;
		}
	}
}

void getFriend(char* message, char* out) {
	char username[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	int i = 0, j = 0;
	for (i = 0; i < strlen(message) && message[i] != ' '; i++) {

	}
	i++;
	for (; i < strlen(message); i++) {
		username[j] = message[i];
		j++;
	}
	memcpy(out, username, BUFF_SIZE);
}



void share(char* message, char* out) {
	char username[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	int i = 0, j = 0;
	for (i = 0; i < strlen(message) && message[i] != ' '; i++) {

	}
	i++;
	for (; i < strlen(message); i++) {
		username[j] = message[i];
		j++;
	}
	memcpy(out, username, BUFF_SIZE);
}

void addFriend(char* message, char* out,int id) {
	char username[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	int i = 0, j = 0;
	for (i = 0; i < strlen(message) && message[i] != ' '; i++) {

	}
	i++;
	for (; i < strlen(message); i++) {
		username[j] = message[i];
		j++;
	}
	memcpy(out, username, BUFF_SIZE);
	
	Friend newFriend;
	newFriend.id_account1 = getIdFromUsername(username);
	newFriend.id_account2 = getIdFromUsername(arrClient[i].username);
	arrFriend.push_back(newFriend);

}

void acceptFriend(char* message, char* out,int id) {
	char username[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	int i = 0, j = 0;
	for (i = 0; i < strlen(message) && message[i] != ' '; i++) {

	}
	i++;
	for (; i < strlen(message); i++) {
		username[j] = message[i];
		j++;
	}
	Friend newFriend;
	newFriend.id_account1 = getIdFromUsername(username);
	newFriend.id_account2 = getIdFromUsername(arrClient[i].username);
	arrFriend.push_back(newFriend);
	memcpy(out, "100",BUFF_SIZE);
}

void declineFriend(char* message, char* out) {
	char username[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	int i = 0, j = 0;
	for (i = 0; i < strlen(message) && message[i] != ' '; i++) {

	}
	i++;
	for (; i < strlen(message); i++) {
		username[j] = message[i];
		j++;
	}
	memcpy(out, username, BUFF_SIZE);
}

void savePlace(char *message, char *output) {
	char nameplace[BUFF_SIZE];
	memset(nameplace, 0, sizeof(nameplace));
	for (int i = strlen(message) - 1; i >= 0 && message[i] != ' '; i--) {
		nameplace[i] = message[i];
	}
	memcpy(output, nameplace, BUFF_SIZE);
}

void echoProcess(char* message, char* out) {
	memcpy(out, message, BUFF_SIZE);
}

/* The send() wrapper function*/
int Send(SOCKET s, char *buff, int size, int flags) {
	int n;

	n = send(s, buff, size, flags);
	if (n == SOCKET_ERROR)
		printf("Error: %", WSAGetLastError());

	return n;
}

unsigned __stdcall echoThread(void *param) {

	int index = (int)param;
	int id = index;
	index = index*FD_SETSIZE;

	timeval tv;
	tv.tv_usec = 1000;
	char rbuff[BUFF_SIZE];
	char sbuff[BUFF_SIZE];
	char buff[BUFF_SIZE], buffHeader[BUFF_SIZE];
	int ret, nEvents, check = false;

	FD_ZERO(&initfds[id]);


	while (1) {

		memset(rbuff, 0, sizeof(rbuff));
		memset(sbuff, 0, sizeof(sbuff));
		check = false;

		for (int i = index; i < index + FD_SETSIZE; i++) {
			EnterCriticalSection(&critical);

			if (arrClient[i].s != 0 && !FD_ISSET(arrClient[i].s, &initfds[id])) {

				FD_SET(arrClient[i].s, &initfds[id]);
			}
			if (arrClient[i].s != 0) check = true;
			LeaveCriticalSection(&critical);
		}

		readfsd[id] = initfds[id];
		writefds[id] = initfds[id];



		if (!check) continue;

		nEvents = select(0, &readfsd[id], &writefds[id], 0, &tv);


		if (nEvents < 0) {
			printf("\n1Error! Cannot poll socket: %d", WSAGetLastError());
			break;
		}

		for (int i = index; i < index + FD_SETSIZE; i++) {


			if (arrClient[i].s == 0) continue;

			if (FD_ISSET(arrClient[i].s, &readfsd[id])) {
				ret = Receive(arrClient[i].s, rbuff, BUFF_SIZE, 0);
				if (ret <= 0) {
					EnterCriticalSection(&critical);
					FD_CLR(arrClient[i].s, &initfds[id]);

					closesocket(arrClient[i].s);
					arrClient[i].s = 0;

					strcpy_s(arrClient[i].username, "");
					LeaveCriticalSection(&critical);
					memset(queueBuff[i], 0, sizeof(queueBuff[i]));
					continue;
				}
				else {
					rbuff[ret] = 0;
					strcat_s(queueBuff[i], rbuff);

				}


			}
			else if (FD_ISSET(arrClient[i].s, &writefds[id])) {
				char *temp = strstr(queueBuff[i], ENDING_DELIMITER);
				if (!temp) continue;
				else {
					memset(buff, 0, sizeof(buff));
					int l1 = strlen(queueBuff[i]), l2 = strlen(temp);
					for (int j = 0; j < l1 - l2; j++) {
						buff[j] = queueBuff[i][j];
					}
					buff[l1 - l2] = 0;
					strcpy_s(queueBuff[i], temp + 2);
					printf("noi dung xu ly:%s\n", buff);

					getHeader(buff, buffHeader);
					printf("header: %s", buffHeader);

					if (!strcmp(buffHeader, "echo")) {
						echoProcess(buff, sbuff);
					}
					else if (!strcmp(buffHeader, "GETFR")) {
						getFriend(buff, sbuff);
					}
					else if (!strcmp(buffHeader, "ADDFR")) {
						addFriend(buff, sbuff,i);
					}
					else if (!strcmp(buffHeader, "ACCEPT")) {
						acceptFriend(buff, sbuff,i);
					}
					else if (!strcmp(buffHeader, "DECLINE")) {
						declineFriend(buff, sbuff);
					}
					else if (!strcmp(buffHeader, "SHARE")) {

					}

					printf("\nnoi dung gui lai %s\n", sbuff);
					Send(arrClient[i].s, sbuff, strlen(sbuff), 0);
				}

			}



		}


	}
	return 0;
}

int main(int argc, char* argv[])
{
	const int SERVER_PORT = atoi(argv[1]);
	
	for (int i = 0; i < MAX_CLIENT; i++) {
		arrClient[i].s = 0;
		strcpy_s(arrClient[i].username, "");
	}

	memset(queueBuff, 0, sizeof(queueBuff));

	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	//Step 2: Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);
	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}

	//Step 4: Listen request from client

	if (listen(listenSock, 10)) {
		printf("Error! Cannot listen.");
		_getch();
		return 0;
	}

	printf("Server started!\n");

	//Step 5: Communicate with client
	SOCKET connSock;
	sockaddr_in clientAddr;
	char clientIP[INET_ADDRSTRLEN];
	int clientAddrLen = sizeof(clientAddr), clientPort;

	InitializeCriticalSection(&critical);


	while (1) {
		connSock = accept(listenSock, (sockaddr *)& clientAddr, &clientAddrLen);
		if (connSock == SOCKET_ERROR)
			printf("Error %d: Cannot permit incoming connection.\n", WSAGetLastError());
		else {
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);
			printf("Accept incoming connection from %s:%d\n", clientIP, clientPort);
			int i;
			for (i = 0; i < MAX_CLIENT; i++) {
				if (arrClient[i].s == 0) {
					arrClient[i].s = connSock;
					strcpy_s(arrClient[i].clientIP, clientIP);
					arrClient[i].clientPort = clientPort;
					if (cnt*FD_SETSIZE <= i) {
						printf("____________________________\nTao moi thread \n");

						_beginthreadex(0, 0, echoThread, (void *)cnt, 0, 0); //start thread
						EnterCriticalSection(&critical);
						cnt++;
						LeaveCriticalSection(&critical);
					}
					break;

				}
			}

			if (i == MAX_CLIENT) {
				printf("Error: Cannot response more client.\n");
				closesocket(connSock);
			}

		}
	}

	DeleteCriticalSection(&critical);

	//close socket
	closesocket(listenSock);
	//terminate socket
	WSACleanup();

	return 0;
}
