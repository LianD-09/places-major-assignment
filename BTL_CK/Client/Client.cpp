// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "conio.h"
#include "string.h"
#include "stdlib.h"
#include "winsock2.h"
#include "ws2tcpip.h"

#define ENDING_DELIMITER "\r\n"
#define BUFF_SIZE 2048

#pragma comment(lib,"Ws2_32.lib")

int isLogin = 0, isGetListFriend = 0;
char glo_username[BUFF_SIZE];

boolean processLogin(char* out) {
	char username[BUFF_SIZE], password[BUFF_SIZE], buff[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(buff, 0, sizeof(buff));
	printf("Enter your username: ");
	gets_s(username, sizeof(username));
	if (strlen(username) <= 0) return false;
	printf("Enter your password: ");
	gets_s(password, sizeof(password));
	if (strlen(password) <= 0) return false;
	strcat_s(buff, "LOGIN*");
	strcat_s(buff, username);
	strcat_s(buff, "*");
	strcat_s(buff, password);
	memset(glo_username, 0, sizeof(glo_username));
	memcpy(glo_username, username, BUFF_SIZE);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

boolean processRegister(char* out) {
	char username[BUFF_SIZE], password[BUFF_SIZE], buff[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(buff, 0, sizeof(buff));
	printf("Enter your usrname: ");
	gets_s(username, sizeof(username));
	if (strlen(username) <= 0) return false;
	printf("Enter your password: ");
	gets_s(password, sizeof(password));
	if (strlen(password) <= 0) return false;

	strcat_s(buff, "SIGNUP*");
	strcat_s(buff, username);
	strcat_s(buff, "*");
	strcat_s(buff, password);

	memcpy(out, buff, BUFF_SIZE);
	return true;
}

void processLogout(char *out) {

	memcpy(out, "LOGOUT", BUFF_SIZE);
}

boolean processAddNewFavouritePlace(char *out) {
	char id_plc[BUFF_SIZE], buff[BUFF_SIZE];
	memset(id_plc, 0, sizeof(id_plc));
	memset(buff, 0, sizeof(buff));
	printf("Enter ID place: ");
	gets_s(id_plc);
	if (strlen(id_plc) <= 0) return false;
	strcat_s(buff, "SAVEPLC*");
	strcat_s(buff, id_plc);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

void processGetFavouritePlace(char *out) {
	char buff[BUFF_SIZE];
	memset(buff, 0, sizeof(BUFF_SIZE));
	strcat_s(buff, "GETFAV*");
	strcat_s(buff, glo_username);
	memcpy(out, buff, BUFF_SIZE);
}

bool processRemovePlaceFromFavouritePlace(char *out) {
	char id_plc[BUFF_SIZE], buff[BUFF_SIZE];
	memset(id_plc, 0, sizeof(id_plc));
	memset(buff, 0, sizeof(buff));
	printf("Enter ID place: ");
	gets_s(id_plc);
	if (strlen(id_plc) <= 0) return false;
	strcat_s(buff, "DELETE*");
	strcat_s(buff, id_plc);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}


void processGetListFriend(char *out) {
	char buff[BUFF_SIZE];
	memset(buff, 0, sizeof(BUFF_SIZE));
	strcat_s(buff, "GETFR*");
	strcat_s(buff, glo_username);
	memcpy(out, buff, BUFF_SIZE);
}
//new thong diep
void processGetListRequestFriend(char *out) {
	char buff[BUFF_SIZE];
	memset(buff, 0, sizeof(BUFF_SIZE));
	strcat_s(buff, "GETRESF*");
	strcat_s(buff, glo_username);
	memcpy(out, buff, BUFF_SIZE);
}

boolean processAddRequestFriend(char *out) {
	char username[BUFF_SIZE], buff[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	memset(buff, 0, sizeof(buff));

	while (1) {
		printf("Enter your your username's friend: ");
		gets_s(username);
		if (strlen(username) <= 0) return false;
		if (!strcmp(username, glo_username)) {
			printf("This is your account!\n");
			continue;
		}
		break;
	}

	strcat_s(buff, "ADDFR*");
	strcat_s(buff, username);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

boolean processAcceptRequestFriend(char *out) {
	char username[BUFF_SIZE], buff[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	memset(buff, 0, sizeof(buff));
	printf("Enter username you want to accept friend : ");
	gets_s(username);
	if (strlen(username) <= 0) return false;
	strcat_s(buff, "ACCEPT*");
	strcat_s(buff, username);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

boolean processDeclineRequestFriend(char *out) {
	char username[BUFF_SIZE], buff[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	memset(buff, 0, sizeof(buff));
	printf("Enter username you want to decline friend : ");
	gets_s(username);
	if (strlen(username) <= 0) return false;
	strcat_s(buff, "DECLINE*");
	strcat_s(buff, username);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

boolean processSharePlaceToFriend(char *out) {
	char id_plc[BUFF_SIZE], username[BUFF_SIZE], buff[BUFF_SIZE];
	memset(username, 0, sizeof(username));
	memset(id_plc, 0, sizeof(id_plc));
	memset(buff, 0, sizeof(buff));
	printf("Enter ID place: ");
	gets_s(id_plc);
	if (strlen(id_plc) <= 0) return false;
	printf("Enter your your username's friend: ");
	gets_s(username);
	if (strlen(username) <= 0) return false;
	strcat_s(buff, "SHARE*");
	strcat_s(buff, id_plc);
	strcat_s(buff, "*");
	strcat_s(buff, username);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

boolean processAddCategory(char *out) {
	char category[BUFF_SIZE], buff[BUFF_SIZE];
	memset(category, 0, sizeof(category));
	memset(buff, 0, sizeof(buff));
	printf("Enter name of category: ");
	gets_s(category);
	if (strlen(category) <= 0) return false;
	strcat_s(buff, "ADDCTG*");
	strcat_s(buff, category);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

boolean processAddPLC(char *out) {
	char id_ctg[BUFF_SIZE], place[BUFF_SIZE], buff[BUFF_SIZE];
	memset(id_ctg, 0, sizeof(id_ctg));
	memset(place, 0, sizeof(place));
	memset(buff, 0, sizeof(buff));
	printf("Enter ID category: ");
	gets_s(id_ctg);
	if (strlen(id_ctg) <= 0) return false;
	printf("Enter your name of place: ");
	gets_s(place);
	if (strlen(place) <= 0) return false;
	strcat_s(buff, "ADDPLC*");
	strcat_s(buff, place);
	strcat_s(buff, "*");
	strcat_s(buff, id_ctg);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

boolean processgetPlaces(char *out) {
	char category[BUFF_SIZE], buff[BUFF_SIZE];
	memset(category, 0, sizeof(category));
	memset(buff, 0, sizeof(buff));
	printf("Enter id of category: ");
	gets_s(category);
	if (strlen(category) <= 0) return false;
	strcat_s(buff, "GETPLC*");
	strcat_s(buff, category);
	memcpy(out, buff, BUFF_SIZE);
	return true;
}

void processStoreFavorite(char *out) {
	char buff[BUFF_SIZE];
	memset(buff, 0, sizeof(BUFF_SIZE));
	strcat_s(buff, "STORE*");
	strcat_s(buff, glo_username);
	memcpy(out, buff, BUFF_SIZE);
}

void processRestoreFavorite(char *out) {
	char buff[BUFF_SIZE];
	memset(buff, 0, sizeof(BUFF_SIZE));
	strcat_s(buff, "RESTORE*");
	strcat_s(buff, glo_username);
	memcpy(out, buff, BUFF_SIZE);
}

void getListCategory(char *out) {
	char buff[BUFF_SIZE];
	memset(buff, 0, sizeof(BUFF_SIZE));
	strcat_s(buff, "GETCTG*");
	memcpy(out, buff, BUFF_SIZE);
}

int main(int argc, char* argv[])
{
	const int SERVER_PORT = atoi(argv[2]);
	const char* SERVER_ADDR = argv[1];
	//Step 1: Initiate Winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}
	//Step 2: Construct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	// Set time-out for receiving
	int tv = 10000;
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	//Step 3: Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	//Step 4: Request to connect server
	if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("Error %d: Cannot connect server.", WSAGetLastError());
		return 0;
	}
	printf("Connected server!\n");

	//Step 5: Communicate with server
	char sbuff[BUFF_SIZE], rbuff[BUFF_SIZE], content[BUFF_SIZE];
	int ret, messageLen;
	while (1) {
		system("CLS");
		memset(sbuff, 0, sizeof(sbuff));
		memset(rbuff, 0, sizeof(rbuff));
		memset(content, 0, sizeof(content));
		if (!isLogin) {
			char buff[BUFF_SIZE];
			memset(buff, 0, sizeof(buff));
			printf("Welcome!");
			printf("\n----------------------------------------------\n\n");
			printf("1. Login\n");
			printf("2. Register \n");
			printf("3. Exit\n");
			printf("\n");
			printf("Enter your option: ");
			gets_s(buff);
			if (!strcmp(buff, "1")) {
				if (!processLogin(sbuff)) {
					continue;
				}
			}
			else if (!strcmp(buff, "2")) {
				if (!processRegister(sbuff)) {
					continue;
				}
			}
			else if (!strcmp(buff, "3")) {
				break;
			}
			else {
				printf("Invalid syntax\n");
				_getch();
				continue;
			}
		}
		else {
			if (!isGetListFriend) {
				char buff[BUFF_SIZE];
				memset(buff, 0, sizeof(buff));
				printf("Welcome %s", glo_username);
				printf("\n----------------------------------------------\n\n");
				printf("1. Logout\n");
				printf("2. Add place to favourites list\n");
				printf("3. Get favourite list\n");
				printf("4. Remove place from favourites list\n");
				printf("5. Get friend list\n");
				printf("6. Get friend request\n");//them accep va decline sau cai nay
				printf("7. Share place to friend\n");
				printf("8. Add new place\n");
				printf("9. Add new category\n");
				printf("10. Get list category\n");
				printf("11. Get list place\n");
				printf("12. Add new friend\n");
				printf("13. Backup favorite list\n");
				printf("14. Restore favorite list\n");
				printf("15. Exit\n");
				printf("\n");
				printf("Choose: ");
				gets_s(buff);

				if (!strcmp(buff, "1")) {
					processLogout(sbuff);
				}
				else if (!strcmp(buff, "2")) {
					if (!processAddNewFavouritePlace(sbuff)) {
						continue;
					}

				}
				else if (!strcmp(buff, "3")) {
					processGetFavouritePlace(sbuff);
				}
				else if (!strcmp(buff, "4")) {
					if (!processRemovePlaceFromFavouritePlace(sbuff)) {
						continue;
					}
				}
				else if (!strcmp(buff, "5")) {
					processGetListFriend(sbuff);
				}
				else if (!strcmp(buff, "6")) {
					processGetListRequestFriend(sbuff);
				}
				else if (!strcmp(buff, "7")) {
					if (!processSharePlaceToFriend(sbuff)) {
						continue;
					}
				}
				else if (!strcmp(buff, "8")) {
					if (!processAddPLC(sbuff)) {
						continue;
					}
				}
				else if (!strcmp(buff, "9")) {
					if (!processAddCategory(sbuff)) {
						continue;
					}
				}
				else if (!strcmp(buff, "10")) {
					getListCategory(sbuff);
				}
				else if (!strcmp(buff, "11")) {
					if (!processgetPlaces(sbuff)) {
						continue;
					}
				}
				else  if (!strcmp(buff, "12")) {
					if (!processAddRequestFriend(sbuff)) {
						continue;
					}
				}
				else if (!strcmp(buff, "13")) {
					processStoreFavorite(sbuff);
				}
				else if (!strcmp(buff, "14")) {
					processRestoreFavorite(sbuff);
				}
				else if (!strcmp(buff, "15")) {
					break;
				}
				else {
					printf("Invalid syntax\n");
					_getch();
					continue;
				}
			}
			else {
				char buff[BUFF_SIZE];
				memset(buff, 0, sizeof(buff));
				printf("Enter 1 to accept request friend\n");
				printf("Enter 2 to deline request friend\n");
				printf("Enter 3 to return\n");
				printf("\n");
				printf("Choose: ");
				gets_s(buff);
				if (!strcmp(buff, "1")) {
					if (!processAcceptRequestFriend(sbuff)) {
						continue;
					}
				}
				else if (!strcmp(buff, "2")) {
					if (!processDeclineRequestFriend(sbuff)) {
						continue;
					}
				}
				else if (!strcmp(buff, "3")) {
					isGetListFriend = 0;
					continue;
				}
				else {
					printf("Invalid syntax\n");
					_getch();
					continue;
				}
			}
		}
		strcat_s(sbuff, ENDING_DELIMITER);

		ret = send(client, sbuff, strlen(sbuff), 0);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot send data.", WSAGetLastError());
		}

		printf("\n--------------\n\n");

		ret = recv(client, rbuff, BUFF_SIZE, 0);

		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT) {
				printf("Time-out!");
			}
			else printf("Error %d: Cannot receive data\n", WSAGetLastError());
		}
		else if (strlen(rbuff) > 0) {
			rbuff[ret] = 0;
			char res[BUFF_SIZE];
			memset(res, BUFF_SIZE, 0);
			if (strlen(rbuff) > 3) {
				char code[BUFF_SIZE];
				memset(code, 0, sizeof(code));
				for (int i = 0; rbuff[i] != '*'; i++) {
					code[i] = rbuff[i];
				}

				if (!strcmp(code, "150")) {
					isGetListFriend = 1;
				}
				else if (!strcmp(code, "010")) {
					isLogin = 1;
				}
				strcpy_s(res, "\n");
				if (!strcmp(code, "010")) {
					strcat_s(res, "Login successful\n");
				}
				strcat_s(res, rbuff + 4);
			}
			else {
				if (!strcmp(rbuff, "010")) {
					strcpy_s(res, "Login successful");
					isLogin = 1;
				}
				else if (!strcmp(rbuff, "011")) strcpy_s(res, "password wrong");
				else if (!strcmp(rbuff, "012")) strcpy_s(res, "username wrong");
				else if (!strcmp(rbuff, "013")) strcpy_s(res, "Login fail! Account has logined");
				else if (!strcmp(rbuff, "020")) {
					strcpy_s(res, "Logout successful");
					isLogin = 0;
				}
				else if (!strcmp(rbuff, "021")) strcpy_s(res, "You haven't logined yet");
				else if (!strcmp(rbuff, "030")) strcpy_s(res, "Register successful");
				else if (!strcmp(rbuff, "031")) strcpy_s(res, "Register fail! Account existed");
				else if (!strcmp(rbuff, "040")) strcpy_s(res, "Successful! This place has been added to your favourite list");
				else if (!strcmp(rbuff, "041")) strcpy_s(res, "Place has existed in your favourite list");
				else if (!strcmp(rbuff, "042")) strcpy_s(res, "Place hasn't existed");
				else if (!strcmp(rbuff, "060")) strcpy_s(res, "Remove successful");
				else if (!strcmp(rbuff, "061")) strcpy_s(res, "Place has't existed in favourite list");
				else if (!strcmp(rbuff, "090")) strcpy_s(res, "Sent request successful");
				else if (!strcmp(rbuff, "091")) strcpy_s(res, "Friend existed");
				else if (!strcmp(rbuff, "092")) strcpy_s(res, "Username hasn't existed");
				else if (!strcmp(rbuff, "100")) strcpy_s(res, "Accept succesful");
				else if (!strcmp(rbuff, "110")) strcpy_s(res, "Decline succesful");
				else if (!strcmp(rbuff, "120")) strcpy_s(res, "Share succesful");
				else if (!strcmp(rbuff, "998")) strcpy_s(res, "Account hasn't request friend to you");
				else if (!strcmp(rbuff, "121")) strcpy_s(res, "Account isn't your friend");
				else if (!strcmp(rbuff, "130")) strcpy_s(res, "Add place successful");
				else if (!strcmp(rbuff, "131")) strcpy_s(res, "Place has exited");
				else if (!strcmp(rbuff, "132")) strcpy_s(res, "category is not exited");
				else if (!strcmp(rbuff, "140")) strcpy_s(res, "Add category successful");
				else if (!strcmp(rbuff, "141")) strcpy_s(res, "Category is exited");
				else if (!strcmp(rbuff, "999")) strcpy_s(res, "Invalid syntax!");
				else if (!strcmp(rbuff, "052")) strcpy_s(res, "There's no place in this category");
				else if (!strcmp(rbuff, "051")) strcpy_s(res, "Empty list");
				else if (!strcmp(rbuff, "123")) strcpy_s(res, "Your friend has add this place in favorite list");
				else if (!strcmp(rbuff, "122")) strcpy_s(res, "Ban da chia se dia diem nay cho ban be");
				else if (!strcmp(rbuff, "150")) strcpy_s(res, "Backup successful");
				else if (!strcmp(rbuff, "160")) strcpy_s(res, "Restore successful");
				else if (!strcmp(rbuff, "161")) strcpy_s(res, "There's no data to recover");

			}
			printf("Receive from server:\n%s\n", res);
			_getch();
		}
	}

	//Step 6: Close socket
	closesocket(client);
	//Step 7: Terminate Winsock
	WSACleanup();


	return 0;
}