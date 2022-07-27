#include "stdafx.h"
#include "direct.h"
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
#include <iostream>
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable : 4996)
#define BUFF_SIZE 2048
#define MAX_CLIENT 4069
#define MAX_ACCOUNT 2000
#define STORE_FOLDER ".\\Store"

#define ENDING_DELIMITER "\r\n"

using namespace std;

int globalAccountId = 1;
int globalPlaceId = 1;
int globalCategoryId = 1;


typedef struct Client {
	SOCKET s;
	char clientIP[INET_ADDRSTRLEN];
	int clientPort;
	char username[BUFF_SIZE];
}Client;


typedef struct Account {
	int id_account;
	char userName[BUFF_SIZE];
	char password[BUFF_SIZE];

	Account(char* _username, char* _password) {
		id_account = globalAccountId;
		globalAccountId++;
		memset(userName, 0, sizeof(userName));
		memset(password, 0, sizeof(password));
		strcpy_s(userName, _username);
		strcpy_s(password, _password);

	}

	Account(int id, char* _username, char* _password) {
		id_account = id;
		globalAccountId++;
		memset(userName, 0, sizeof(userName));
		memset(password, 0, sizeof(password));
		strcpy_s(userName, _username);
		strcpy_s(password, _password);
	}
}Account;


typedef struct Category {
	int id_category;
	char name[BUFF_SIZE];

	Category(char* _name) {
		id_category = globalCategoryId;
		globalCategoryId++;
		memset(name, 0, sizeof(name));
		strcpy_s(name, _name);
	}

	Category(int cateId, char* _name) {
		id_category = cateId;
		globalCategoryId++;
		memset(name, 0, sizeof(name));
		strcpy_s(name, _name);
	}
}Category;


typedef struct Place {
	int id_place;
	int id_category;
	char name[BUFF_SIZE];

	Place(int cateId, char* _name) {
		id_place = globalPlaceId;
		globalPlaceId++;
		id_category = cateId;
		memset(name, 0, sizeof(name));
		strcpy_s(name, _name);
	}

	Place(int placeId, int cateId, char* _name) {
		id_place = placeId;
		globalPlaceId++;
		id_category = cateId;
		memset(name, 0, sizeof(name));
		strcpy_s(name, _name);
	}
}Place;

typedef struct Friend {
	int id_account1;
	int id_account2;

	Friend(int accountId1, int accountId2) {
		id_account1 = accountId1;
		id_account2 = accountId2;
	}
}Friend;

typedef struct Favorite {
	int id_account;
	int id_place;
	int id_friend;

	Favorite(int accountId, int placeId, int f) {
		id_account = accountId;
		id_place = placeId;
		id_friend = f;
	}
};

typedef struct Notification {
	int tag;// 0 is Tag, 1 is add friend
	int id_account1;
	int id_account2;
	int id_place;
	int isSeen;//0 no seen/seen chua xu ly, 1 seen va da xu ly 
	Notification(int _type, int accountId1, int accountId2, int placeId, int seen) {
		tag = _type;
		id_account1 = accountId1;
		id_account2 = accountId2;
		id_place = placeId;
		isSeen = seen;
	}
};

vector<Account> arrAccount;
vector<Category> arrCategory;
vector<Place> arrPlace;
vector<Friend> arrFriend;
vector<Favorite> arrFavorite;
vector<Notification> arrNotification;

char AccountFile[] = ".\\AccountFile.csv";
char CategoryFile[BUFF_SIZE] = ".\\CategoryFile.csv";
char PlaceFile[BUFF_SIZE] = ".\\PlaceFile.csv";
char FriendFile[BUFF_SIZE] = ".\\FriendFile.csv";
char FavoriteFile[BUFF_SIZE] = ".\\FavoriteFile.csv";
char NotificationFile[BUFF_SIZE] = ".\\NotificationFile.csv";

CRITICAL_SECTION critical;
int cnt = 0;
Client arrClient[MAX_CLIENT * 2];

char queueBuff[MAX_CLIENT][11 * BUFF_SIZE];
fd_set readfsd[10000], initfds[10000], writefds[10000];

void ReadAccountFile() {
	FILE* fp;
	char username[BUFF_SIZE];
	char password[BUFF_SIZE];
	int id;
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));

	fp = fopen(AccountFile, "a+");

	if (fp == NULL) {
		printf("%s file not open!\n", AccountFile);
		return;
	}

	while (fscanf(fp, "%[^,],%[^,],%d\n", username, password, &id) != EOF) {
		username[strlen(username)] = 0;
		cout << "username: " << username << " " << strlen(username) << endl;
		cout << "password: " << password << " " << strlen(password) << endl;
		arrAccount.push_back(Account(id, username, password));
	}
	cout << endl;
	fclose(fp);
}

void ReadPlaceFile() {
	FILE* fp;
	char place[BUFF_SIZE];
	int placeId;
	int cateId;
	memset(place, 0, sizeof(place));

	fp = fopen(PlaceFile, "a+");

	if (fp == NULL) {
		printf("%s file not open!\n", PlaceFile);
		return;
	}

	while (fscanf(fp, "%[^,],%d,%d\n", place, &placeId, &cateId) != EOF) {
		arrPlace.push_back(Place(placeId, cateId, place));
	}
	cout << endl;
	fclose(fp);
}

void ReadCategoryFile() {
	FILE* fp;
	char category[BUFF_SIZE];
	int cateId;
	memset(category, 0, sizeof(category));

	fp = fopen(CategoryFile, "a+");

	if (fp == NULL) {
		printf("%s file not open!\n", CategoryFile);
		return;
	}

	while (fscanf(fp, "%[^,],%d\n", category, &cateId) != EOF) {
		arrCategory.push_back(Category(cateId, category));
	}
	cout << endl;
	fclose(fp);
}

void ReadFavoriteFile() {
	FILE* fp;
	int accountId;
	int placeId, f;

	fp = fopen(FavoriteFile, "a+");

	if (fp == NULL) {
		printf("%s file not open!\n", FavoriteFile);
		return;
	}

	while (fscanf(fp, "%d,%d,%d\n", &accountId, &placeId, &f) != EOF) {
		arrFavorite.push_back(Favorite(accountId, placeId, f));
	}
	cout << endl;
	fclose(fp);
}

void ReadFriendFile() {
	FILE* fp;
	int accountId1;
	int accountId2;

	fp = fopen(FriendFile, "a+");

	if (fp == NULL) {
		printf("%s file not open!\n", FriendFile);
		return;
	}

	while (fscanf(fp, "%d,%d\n", &accountId1, &accountId2) != EOF) {
		arrFriend.push_back(Friend(accountId1, accountId2));
	}
	cout << endl;
	fclose(fp);
}

void ReadNotificationFile() {
	FILE* fp;
	int type;
	int accountId1;
	int accountId2;
	int placeId;
	int isSeen;

	fp = fopen(NotificationFile, "a+");

	if (fp == NULL) {
		printf("%s file not open!\n", NotificationFile);
		return;
	}

	while (fscanf(fp, "%d,%d,%d,%d,%d\n", &type, &accountId1, &accountId2, &placeId, &isSeen) != EOF) {
		arrNotification.push_back(Notification(type, accountId1, accountId2, placeId, isSeen));
	}
	cout << endl;
	fclose(fp);
}

void SaveData(char* filePath, char* data) {
	EnterCriticalSection(&critical);
	FILE* fp = fopen(filePath, "a+");
	if (fp == NULL) {
		cout << "Cannot open file to write" << endl;
		return;
	}
	cout << "Saved" << endl;

	fprintf(fp, "%s\n", data);
	fclose(fp);
	LeaveCriticalSection(&critical);
}

void rewriteFavoriteFile() {
	EnterCriticalSection(&critical);
	FILE* ftemp;
	char data[BUFF_SIZE];
	memset(data, 0, sizeof(data));
	char temp[] = "Temp.csv";

	ftemp = fopen(temp, "a+");

	if (ftemp == NULL) {
		cout << "Cannot open file to write" << endl;
		return;
	}

	//Read each line by fscanf and write to temp file except delete line
	char buff[10];
	for (auto v : arrFavorite) {
		_itoa(v.id_account, buff, 10);
		strcat_s(data, buff);
		strcat_s(data, ",");

		_itoa(v.id_place, buff, 10);
		strcat_s(data, buff);
		strcat_s(data, ",");

		_itoa(v.id_friend, buff, 10);
		strcat_s(data, buff);
		strcat_s(data, "\n");

	}
	fprintf(ftemp, "%s", data);
	cout << "Rewrite done!" << endl;
	fclose(ftemp); // Close file Temp.csv

				   // Remove old file and rename new file
	remove(FavoriteFile);
	rename(temp, FavoriteFile);
	LeaveCriticalSection(&critical);
}

void updateNotificationfile() {
	EnterCriticalSection(&critical);
	FILE* fp = fopen(NotificationFile, "w+");
	if (fp == NULL) {
		cout << "Cannot open file to write" << endl;
		return;
	}
	cout << "Saved" << endl;
	char buff[BUFF_SIZE];
	memset(buff, 0, sizeof(buff));
	for (int i = 0; i < arrNotification.size(); i++) {
		char data[BUFF_SIZE];
		memset(data, 0, sizeof(data));
		char num[10];
		memset(num, 0, sizeof(num));
		_itoa(arrNotification[i].tag, num, 10);
		strcat_s(data, num);
		strcat(data, ",");
		memset(num, 0, sizeof(num));
		_itoa(arrNotification[i].id_account1, num, 10);
		strcat_s(data, num);
		strcat(data, ",");
		memset(num, 0, sizeof(num));
		_itoa(arrNotification[i].id_account2, num, 10);
		strcat_s(data, num);
		strcat(data, ",");
		memset(num, 0, sizeof(num));
		_itoa(arrNotification[i].id_place, num, 10);
		strcat_s(data, num);
		strcat(data, ",");
		memset(num, 0, sizeof(num));
		_itoa(arrNotification[i].isSeen, num, 10);
		strcat_s(data, num);
		strcat(data, "\n");
		strcat(buff, data);
	}
	fprintf(fp, "%s", buff);
	fclose(fp);
	LeaveCriticalSection(&critical);
}

void getHighPartBySplitDelimiter(char* message, char* header) {
	char buffHeader[BUFF_SIZE];
	memset(buffHeader, 0, sizeof(buffHeader));
	for (int i = 0; i < strlen(message) && message[i] != '*'; i++) {
		buffHeader[i] = message[i];
	}
	memcpy(header, buffHeader, BUFF_SIZE);

}

void getNotificationTagOfAccount(int id_account, char* out);

void Login(char* buff, char* out, int id) {
	char sbuff[BUFF_SIZE];
	memset(sbuff, 0, sizeof(sbuff));
	if (strcmp(arrClient[id].username, "")) {
		cout << "da dang nhap" << endl;
		strcpy_s(sbuff, "013");
	}
	else {
		int accountId;
		char username[BUFF_SIZE];
		char password[BUFF_SIZE];
		memset(username, 0, sizeof(username));
		memset(password, 0, sizeof(password));
		getHighPartBySplitDelimiter(buff + 6, username);
		getHighPartBySplitDelimiter(buff + 6 + strlen(username) + 1, password);

		cout << endl << username << endl << password << endl;

		int idAccount;
		bool validated = false;
		int length = arrAccount.size();
		for (idAccount = 0; idAccount < length; idAccount++) {
			if (!strcmp(username, arrAccount[idAccount].userName)) {
				if (!strcmp(password, arrAccount[idAccount].password)) {
					accountId = arrAccount[idAccount].id_account;
					validated = true;
				}
				break;
			}
		}
		if (idAccount == length) {
			strcpy_s(sbuff, "012");
		}
		else {
			if (!validated) {
				strcpy_s(sbuff, "011");
			}
			else {
				int i;
				for (i = 0; i < MAX_CLIENT; i++) {
					if (!strcmp(username, arrClient[i].username)) break;
				}
				if (i == MAX_CLIENT) {
					EnterCriticalSection(&critical);
					strcpy_s(arrClient[id].username, username);
					strcpy_s(sbuff, "010*");
					char notificationTag[BUFF_SIZE];
					memset(notificationTag, 0, sizeof(notificationTag));
					getNotificationTagOfAccount(accountId, notificationTag);
					LeaveCriticalSection(&critical);
					strcat_s(sbuff, notificationTag);
				}
				else {
					strcpy_s(sbuff, "013");
				}
			}
		}
	}
	memcpy(out, sbuff, BUFF_SIZE);
}

void Logout(char* buff, char* out, int id) {
	char sbuff[BUFF_SIZE];
	memset(sbuff, 0, sizeof(sbuff));
	if (!strcmp(arrClient[id].username, "")) {
		strcpy_s(sbuff, "021");
	}
	else {
		EnterCriticalSection(&critical);
		strcpy_s(arrClient[id].username, "");
		LeaveCriticalSection(&critical);
		strcpy_s(sbuff, "020");
	}
	memcpy(out, sbuff, BUFF_SIZE);
}

void SignUp(char* buff, char* out, int id) {
	char sbuff[BUFF_SIZE];
	memset(sbuff, 0, sizeof(sbuff));
	if (!strcmp(arrClient[id].username, "")) {
		char username[BUFF_SIZE];
		char password[BUFF_SIZE];
		memset(username, 0, sizeof(username));
		memset(password, 0, sizeof(password));
		getHighPartBySplitDelimiter(buff + 7, username);
		getHighPartBySplitDelimiter(buff + 7 + strlen(username) + 1, password);

		cout << endl << username << endl << password << endl;
		int idAccount;
		int length = arrAccount.size();
		for (idAccount = 0; idAccount < length; idAccount++) {
			if (!strcmp(username, arrAccount[idAccount].userName)) {
				break;
			}
		}
		if (idAccount == length) {
			char data[BUFF_SIZE];
			memset(data, 0, sizeof(data));

			char accountIdString[10];
			memset(accountIdString, 0, sizeof(accountIdString));
			globalAccountId++;
			_itoa(globalAccountId, accountIdString, 10);

			strcat(data, username);
			strcat(data, ",");
			strcat(data, password);
			strcat(data, ",");
			strcat(data, accountIdString);

			SaveData(AccountFile, data);

			strcpy_s(sbuff, "030");

			EnterCriticalSection(&critical);
			arrAccount.push_back(Account(username, password));
			LeaveCriticalSection(&critical);
		}
		else {
			strcpy_s(sbuff, "031");
		}
	}
	else {
		strcpy_s(sbuff, "013");
	}
	memcpy(out, sbuff, BUFF_SIZE);
}

void GetPlacesByCategoryId(char* buff, char* out, int id) {
	char sbuff[BUFF_SIZE];
	memset(sbuff, 0, sizeof(sbuff));
	char categoryId[10];
	memset(categoryId, 0, sizeof(categoryId));
	strncpy_s(categoryId, buff + 7, strlen(buff) - 7);
	int cateIdInt = atoi(categoryId);
	cout << endl << "Category Id: " << cateIdInt << endl;

	char placeList[BUFF_SIZE];
	memset(placeList, 0, sizeof(placeList));
	int length = arrPlace.size();
	if (length == 0) {
		strcpy(sbuff, "051");
	}
	else {
		if (cateIdInt == 0) {
			for (int i = 0; i < length; i++) {
				char placeId[10];
				memset(placeId, 0, sizeof(placeId));
				_itoa(arrPlace[i].id_place, placeId, 10);
				strcat_s(placeList, placeId);
				strcat_s(placeList, " ");
				strcat_s(placeList, arrPlace[i].name);
				strcat_s(placeList, "\n");
			}
			char buff[BUFF_SIZE];
			memset(buff, 0, sizeof(buff));
			strcat_s(buff, "050*");
			strcat_s(buff, placeList);
			strcpy(sbuff, buff);
		}
		else {
			bool cateExist = false;
			for (int i = 0; i < length; i++) {
				if (arrPlace[i].id_category == cateIdInt) {
					cateExist = true;
					char placeId[10];
					memset(placeId, 0, sizeof(placeId));
					_itoa(arrPlace[i].id_place, placeId, 10);
					strcat_s(placeList, placeId);
					strcat_s(placeList, " ");
					strcat_s(placeList, arrPlace[i].name);
					strcat_s(placeList, "\n");
				}
			}
			if (!cateExist) {
				strcpy(sbuff, "052");
			}
			else {
				char buff[BUFF_SIZE];
				memset(buff, 0, sizeof(buff));
				strcat_s(buff, "050*");
				strcat_s(buff, placeList);
				strcpy(sbuff, buff);
			}
		}
	}
	memcpy(out, sbuff, BUFF_SIZE);
}

void AddPlace(char* buff, char* out, int id) {
	char sbuff[BUFF_SIZE];
	memset(sbuff, 0, sizeof(sbuff));
	if (!strcmp(arrClient[id].username, "")) {
		strcpy_s(sbuff, "021");
	}
	else {
		char placeName[BUFF_SIZE];
		char cateId[BUFF_SIZE];
		memset(placeName, 0, sizeof(placeName));
		memset(cateId, 0, sizeof(cateId));
		getHighPartBySplitDelimiter(buff + 7, placeName);
		getHighPartBySplitDelimiter(buff + 7 + strlen(placeName) + 1, cateId);

		int cateIdInt = atoi(cateId);
		int length = arrCategory.size();
		bool isCateEsxist = false;
		for (int i = 0; i < length; i++) {
			if (arrCategory[i].id_category == cateIdInt) {
				isCateEsxist = true;
				break;
			}
		}
		if (!isCateEsxist) {
			strcpy_s(sbuff, "132");
		}
		else {
			bool isPlaceExist = false;
			int arrPlaceLength = arrPlace.size();
			for (int i = 0; i < arrPlaceLength; i++) {
				if (!strcmp(arrPlace[i].name, placeName)) {
					isPlaceExist = true;
					break;
				}
			}
			if (!isPlaceExist) {
				char data[BUFF_SIZE];
				memset(data, 0, sizeof(data));

				char placeIdString[10];
				memset(placeIdString, 0, sizeof(placeIdString));
				_itoa(globalPlaceId, placeIdString, 10);

				strcat(data, placeName);
				strcat(data, ",");
				strcat(data, placeIdString);
				strcat(data, ",");
				strcat(data, cateId);

				SaveData(PlaceFile, data);

				EnterCriticalSection(&critical);
				arrPlace.push_back(Place(cateIdInt, placeName));
				LeaveCriticalSection(&critical);
				strcpy_s(sbuff, "130");
			}
			else {
				strcpy_s(sbuff, "131");
			}
		}
	}
	memcpy(out, sbuff, BUFF_SIZE);
}

void AddCategory(char* buff, char* out, int id) {
	char sbuff[BUFF_SIZE];
	memset(sbuff, 0, sizeof(sbuff));
	if (!strcmp(arrClient[id].username, "")) {
		strcpy_s(sbuff, "021");
	}
	else {
		char cateName[BUFF_SIZE];
		memset(cateName, 0, sizeof(cateName));
		getHighPartBySplitDelimiter(buff + 7, cateName);

		bool isCateExist = false;
		int arrCateLength = arrCategory.size();
		for (int i = 0; i < arrCateLength; i++) {
			if (!strcmp(arrCategory[i].name, cateName)) {
				isCateExist = true;
				break;
			}
		}
		if (!isCateExist) {
			cout << "***";
			char data[BUFF_SIZE];
			memset(data, 0, sizeof(data));

			char cateIdString[10];
			memset(cateIdString, 0, sizeof(cateIdString));
			_itoa(globalCategoryId, cateIdString, 10);

			strcat(data, cateName);
			strcat(data, ",");
			strcat(data, cateIdString);

			SaveData(CategoryFile, data);

			EnterCriticalSection(&critical);
			arrCategory.push_back(Category(cateName));
			LeaveCriticalSection(&critical);
			strcpy_s(sbuff, "140");
		}
		else {
			strcpy_s(sbuff, "141");
		}
	}
	memcpy(out, sbuff, BUFF_SIZE);
}

void getHeader(char* message, char* header) {
	char buffHeader[BUFF_SIZE];
	memset(buffHeader, 0, sizeof(buffHeader));
	for (int i = 0; i < strlen(message) && message[i] != '*'; i++) {
		buffHeader[i] = message[i];
	}
	memcpy(header, buffHeader, BUFF_SIZE);

}

boolean checkIdPlace(int id_place) {
	for (int i = 0; i < arrPlace.size(); i++) {
		if (arrPlace[i].id_place == id_place) {
			return true;
		}
	}
	return false;
}

int Receive(SOCKET s, char* buff, int size, int flags) {
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
	return -1;
}

void getUsernameFromId(int id_account, char* out) {
	for (int i = 0; i < arrAccount.size(); i++) {
		if (id_account == arrAccount[i].id_account) {
			memcpy(out, arrAccount[i].userName, BUFF_SIZE);
			return;
		}
	}
	memcpy(out, "", BUFF_SIZE);
	return;
}

void getPlaceFromId(int id_plc, char* out) {
	for (int i = 0; i < arrPlace.size(); i++) {
		if (arrPlace[i].id_place == id_plc) {
			memcpy(out, arrPlace[i].name, BUFF_SIZE);
			return;
		}
	}
	memcpy(out, "", BUFF_SIZE);
	return;
}

boolean checkFriend(int id_account1, int id_account2) {
	for (int i = 0; i < arrFriend.size(); i++) {
		if (arrFriend[i].id_account1 == id_account1 && arrFriend[i].id_account2 == id_account2 ||
			arrFriend[i].id_account1 == id_account2 && arrFriend[i].id_account2 == id_account1) {
			return true;
		}
	}
	return false;
}

int findNotification(int tag, int id_account1, int id_account2, int id_place, int isSeen) {
	for (int i = 0; i < arrNotification.size(); i++) {
		if (arrNotification[i].tag == tag && arrNotification[i].id_account1 == id_account1
			&& arrNotification[i].id_account2 == id_account2 && arrNotification[i].id_place == id_place
			&& arrNotification[i].isSeen == 0) {
			return i;
		}
	}
	return -1;
}

void getNotificationTagOfAccount(int id_account, char* out) {
	char buff[BUFF_SIZE];
	boolean check = false;
	memset(buff, 0, sizeof(buff));
	for (int i = 0; i < arrNotification.size(); i++) {
		if ((arrNotification[i].id_account2 == id_account) && (arrNotification[i].tag == 0) && (arrNotification[i].isSeen == 0)) {
			char username[BUFF_SIZE], place[BUFF_SIZE];
			getUsernameFromId(arrNotification[i].id_account1, username);
			getPlaceFromId(arrNotification[i].id_place, place);
			strcat_s(buff, username);
			strcat_s(buff, " has tag you in: ");
			strcat_s(buff, place);
			strcat_s(buff, "\n");
			arrNotification[i].isSeen = 1;
			check = true;
		}
	}

	if (check)updateNotificationfile();

	memcpy(out, buff, BUFF_SIZE);
}

void getNotificationFriendOfAccount(char* message, char* out, int id) {
	if (strcmp(arrClient[id].username, "")) {
		char username[BUFF_SIZE];
		memset(username, 0, sizeof(username));
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}

		char buff[BUFF_SIZE];
		memset(buff, 0, sizeof(buff));
		int id_account = getIdFromUsername(username);

		for (int i = 0; i < arrNotification.size(); i++) {
			if (arrNotification[i].id_account2 == id_account && arrNotification[i].tag == 1 && arrNotification[i].isSeen == 0) {
				char username[BUFF_SIZE];
				getUsernameFromId(arrNotification[i].id_account1, username);

				strcat_s(buff, username);
				strcat_s(buff, " sent a friend request to you");
				strcat_s(buff, "\n");
			}
		}
		if (strlen(buff) <= 0) memcpy(out, "051", BUFF_SIZE);
		else {
			char seBuff[BUFF_SIZE];
			memset(seBuff, 0, sizeof(seBuff));
			strcat_s(seBuff, "180*");
			strcat_s(seBuff, buff);
			memcpy(out, seBuff, BUFF_SIZE);
		}
	}
	else {
		memcpy(out, "021", BUFF_SIZE);
	}
}

void getFriend(char* message, char* out, int id) {

	if (strcmp(arrClient[id].username, "")) {
		char username[BUFF_SIZE];
		memset(username, 0, sizeof(username));
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}
		cout << "user name: " << username;

		int id_user = getIdFromUsername(username);
		cout << "id" << id;
		vector<int> lstIdFriend;
		for (int i = 0; i < arrFriend.size(); i++) {
			if (arrFriend[i].id_account1 == id_user) {
				lstIdFriend.push_back(arrFriend[i].id_account2);
			}
			else if (arrFriend[i].id_account2 == id_user) {
				lstIdFriend.push_back(arrFriend[i].id_account1);
			}
		}
		cout << "size" << arrFriend.size() << "size" << lstIdFriend.size() << "\n";

		char lstFriend[BUFF_SIZE];
		memset(lstFriend, 0, sizeof(BUFF_SIZE));
		for (int i = 0; i < lstIdFriend.size(); i++) {
			char buffF[BUFF_SIZE];
			getUsernameFromId(lstIdFriend[i], buffF);
			strcat_s(lstFriend, buffF);
			strcat_s(lstFriend, ",");
		}
		if (strlen(lstFriend) <= 0) memcpy(out, "051", BUFF_SIZE);
		else {
			char seBuff[BUFF_SIZE];
			memset(seBuff, 0, sizeof(seBuff));
			strcat_s(seBuff, "080*");
			strcat_s(seBuff, lstFriend);
			memcpy(out, seBuff, BUFF_SIZE);
		}
	}
	else {
		memcpy(out, "021", BUFF_SIZE);
	}
}

void getListCategory(char* message, char* out, int id) {
	if (strcmp(arrClient[id].username, "")) {
		char buff[BUFF_SIZE];
		memset(buff, 0, sizeof(buff));
		for (int i = 0; i < arrCategory.size(); i++) {
			char Id_ctg[10];
			memset(Id_ctg, 0, sizeof(Id_ctg));
			_itoa(arrCategory[i].id_category, Id_ctg, 10);
			strcat_s(buff, Id_ctg);
			strcat_s(buff, " ");
			strcat_s(buff, arrCategory[i].name);
			strcat_s(buff, "\n");
		}
		if (strlen(buff) <= 0) memcpy(out, "051", BUFF_SIZE);
		else {
			char seBuff[BUFF_SIZE];
			memset(seBuff, 0, sizeof(seBuff));
			strcat_s(seBuff, "160*");
			strcat_s(seBuff, buff);
			memcpy(out, seBuff, BUFF_SIZE);
		}
	}
	else {
		memcpy(out, "021", BUFF_SIZE);
	}
}

void share(char* message, char* out, int id) {
	if (strcmp(arrClient[id].username, "")) {
		char id_place_str[BUFF_SIZE], username[BUFF_SIZE];
		memset(id_place_str, 0, sizeof(id_place_str));
		memset(username, 0, sizeof(username));
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message) && message[i] != '*'; i++) {
			id_place_str[j] = message[i];
			j++;
		}
		int id_plc = atoi(id_place_str);
		i++;
		for (j = 0; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}

		int id_account2 = getIdFromUsername(username);
		printf("usrnameshare:%s", username);
		int id_account1 = getIdFromUsername(arrClient[id].username);
		if (id_account2 == -1) {
			memcpy(out, "092", BUFF_SIZE);
		}
		else {
			if (checkFriend(id_account1, id_account2)) {
				if (checkIdPlace(id_plc)) {

					Notification newNotification = Notification(0, id_account1, id_account2, id_plc, 0);
					if (findNotification(0, id_account1, id_account2, id_plc, 0) == -1) {
						char data[BUFF_SIZE];
						memset(data, 0, sizeof(data));
						char num[10];
						memset(num, 0, sizeof(num));
						_itoa(0, num, 10);
						strcat_s(data, num);
						strcat(data, ",");
						memset(num, 0, sizeof(num));
						_itoa(id_account1, num, 10);
						strcat_s(data, num);
						strcat(data, ",");
						memset(num, 0, sizeof(num));
						_itoa(id_account2, num, 10);
						strcat_s(data, num);
						strcat(data, ",");
						memset(num, 0, sizeof(num));
						_itoa(id_plc, num, 10);
						strcat_s(data, num);
						strcat(data, ",");
						memset(num, 0, sizeof(num));
						_itoa(0, num, 10);
						strcat_s(data, num);
						SaveData(NotificationFile, data);
						EnterCriticalSection(&critical);
						arrNotification.push_back(newNotification);
						LeaveCriticalSection(&critical);
						Favorite newFavourite = Favorite(id_account2, id_plc, id_account1);
						int check = 0;
						for (int i = 0; i < arrFavorite.size(); i++) {
							if (arrFavorite[i].id_account == id_account1 && arrFavorite[i].id_place == id_plc) {
								check = 1;
							}
						}
						if (check != 1) {
							char data[BUFF_SIZE];
							memset(data, 0, sizeof(data));
							char num[10];
							memset(num, 0, sizeof(num));
							_itoa(id_account2, num, 10);
							strcat_s(data, num);
							strcat(data, ",");

							memset(num, 0, sizeof(num));
							_itoa(id_plc, num, 10);
							strcat_s(data, num);
							strcat(data, ",");

							memset(num, 0, sizeof(num));
							_itoa(id_account1, num, 10);
							strcat_s(data, num);
							SaveData(FavoriteFile, data);
							EnterCriticalSection(&critical);
							arrFavorite.push_back(newFavourite);
							LeaveCriticalSection(&critical);
							memcpy(out, "120", BUFF_SIZE);
						}
						else {
							memcpy(out, "123", BUFF_SIZE);
						}
					}
					else {
						memcpy(out, "122", BUFF_SIZE);
					}

				}
				else {
					memcpy(out, "042", BUFF_SIZE);
				}
			}
			else {
				memcpy(out, "121", BUFF_SIZE);
			}
		}
	}
	else {
		memcpy(out, "021", BUFF_SIZE);
	}
}

void addFriend(char* message, char* out, int id) {
	if (strcmp(arrClient[id].username, "")) {
		char username[BUFF_SIZE];
		memset(username, 0, sizeof(username));
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}
		int id_account2 = getIdFromUsername(username);
		int id_account1 = getIdFromUsername(arrClient[id].username);
		if (id_account2 == -1) {
			memcpy(out, "092", BUFF_SIZE);
		}
		else {
			if (checkFriend(id_account1, id_account2)) {
				memcpy(out, "091", BUFF_SIZE);
			}
			else {
				Notification newNotification = Notification(1, id_account1, id_account2, 0, 0);
				char data[BUFF_SIZE];
				memset(data, 0, sizeof(data));
				char num[10];
				memset(num, 0, sizeof(num));
				_itoa(1, num, 10);
				strcat_s(data, num);
				strcat(data, ",");
				memset(num, 0, sizeof(num));
				_itoa(id_account1, num, 10);
				strcat_s(data, num);
				strcat(data, ",");
				memset(num, 0, sizeof(num));
				_itoa(id_account2, num, 10);
				strcat_s(data, num);
				strcat(data, ",");
				memset(num, 0, sizeof(num));
				_itoa(0, num, 10);
				strcat_s(data, num);
				strcat(data, ",");
				memset(num, 0, sizeof(num));
				_itoa(0, num, 10);
				strcat_s(data, num);
				printf(",dataluu %s,", data);
				SaveData(NotificationFile, data);
				EnterCriticalSection(&critical);
				arrNotification.push_back(newNotification);
				LeaveCriticalSection(&critical);
				memcpy(out, "090", BUFF_SIZE);
			}
		}
	}
	else {
		memcpy(out, "021", BUFF_SIZE);
	}
}
//them sua file
void acceptFriend(char* message, char* out, int id) {
	if (strcmp(arrClient[id].username, "")) {
		char username[BUFF_SIZE];
		memset(username, 0, sizeof(username));
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}
		int id_account1 = getIdFromUsername(username), id_account2 = getIdFromUsername(arrClient[id].username);
		if (id_account1 == -1) {
			memcpy(out, "092", BUFF_SIZE);
		}
		else {
			if (checkFriend(id_account1, id_account2)) {
				memcpy(out, "091", BUFF_SIZE);
			}
			else {
				int checkHasNotification = findNotification(1, id_account1, id_account2, 0, 0);
				if (checkHasNotification == -1) {
					memcpy(out, "998", BUFF_SIZE);
				}
				else {
					EnterCriticalSection(&critical);
					arrNotification[checkHasNotification].isSeen = 1;
					LeaveCriticalSection(&critical);
					for (int i = 0; i < arrNotification.size(); i++) {
						if (arrNotification[i].tag == 1 && arrNotification[i].id_account1 == id_account1
							&& arrNotification[i].id_account2 == id_account2) {
							EnterCriticalSection(&critical);
							arrNotification[i].isSeen = 1;
							LeaveCriticalSection(&critical);
						}
					}
					updateNotificationfile();
					Friend newFriend = Friend(id_account1, id_account2);

					char data[BUFF_SIZE];
					memset(data, 0, sizeof(data));
					char num[10];
					memset(num, 0, sizeof(num));
					_itoa(id_account1, num, 10);
					strcat_s(data, num);
					strcat(data, ",");
					memset(num, 0, sizeof(num));
					_itoa(id_account2, num, 10);
					strcat_s(data, num);
					SaveData(FriendFile, data);
					EnterCriticalSection(&critical);
					arrFriend.push_back(newFriend);
					LeaveCriticalSection(&critical);
					memcpy(out, "100", BUFF_SIZE);
				}
			}
		}
	}
	else {
		memcpy(out, "021", BUFF_SIZE);
	}

}

void declineFriend(char* message, char* out, int id) {
	if (strcmp(arrClient[id].username, "")) {
		char username[BUFF_SIZE];
		memset(username, 0, sizeof(username));
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}
		int id_account1 = getIdFromUsername(username), id_account2 = getIdFromUsername(arrClient[id].username);

		if (id_account1 == -1) {
			memcpy(out, "092", BUFF_SIZE);
		}
		else {
			if (checkFriend(id_account1, id_account2)) {
				memcpy(out, "091", BUFF_SIZE);
			}
			else {
				int checkHasNotification = findNotification(1, id_account1, id_account2, 0, 0);
				if (checkHasNotification == -1) {
					memcpy(out, "998", BUFF_SIZE);
				}
				else {
					for (int i = 0; i < arrNotification.size(); i++) {
						if (arrNotification[i].tag == 1 && arrNotification[i].id_account1 == id_account1
							&& arrNotification[i].id_account2 == id_account2) {
							EnterCriticalSection(&critical);
							arrNotification[i].isSeen = 1;
							LeaveCriticalSection(&critical);
						}
					}
					updateNotificationfile();
					memcpy(out, "110", BUFF_SIZE);

				}

			}
		}
	}
	else {
		memcpy(out, "021", BUFF_SIZE);
	}

}

int checkFavorite(int idPlace, int idAccount) {
	for (int i = 0; i < arrFavorite.size(); i++) {
		if (arrFavorite[i].id_account != idAccount) {
			continue;
		}

		if (arrFavorite[i].id_place == idPlace) {
			return i;
		}
	}
	return -1;
}

void savePlace(char* message, char* output, int idClient, int idFriend) {
	char idPlace[BUFF_SIZE]; // Payload
	int id, idAccount = getIdFromUsername(arrClient[idClient].username);
	char data[BUFF_SIZE]; // Contain data save to file
	memset(idPlace, 0, sizeof(idPlace));
	memset(data, 0, sizeof(data));
	if (strcmp(arrClient[idClient].username, "")) {
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			idPlace[j] = message[i];
			j++;
		}
		cout << "Id place" << idPlace << endl;

		id = atoi(idPlace);
		if (!checkIdPlace(id)) {
			memcpy(output, "042", BUFF_SIZE);
			return;
		}
		_itoa(idAccount, data, 10);

		if (checkFavorite(id, idAccount) != -1) {
			memcpy(output, "041", BUFF_SIZE);
		}
		else {
			strcat_s(data, ",");
			strcat_s(data, idPlace);
			strcat_s(data, ",");
			char buff[10];
			_itoa(idFriend, buff, 10);
			strcat_s(data, buff);

			// Add new to current working data
			EnterCriticalSection(&critical);
			arrFavorite.push_back(Favorite(idAccount, id, idFriend));
			LeaveCriticalSection(&critical);
			cout << data << endl;
			SaveData(FavoriteFile, data);
			memcpy(output, "040", BUFF_SIZE);
			return;
		}
	}
	else {
		memcpy(output, "021", BUFF_SIZE);
	}
}

void deletePlace(char* message, char* output, int idClient) {
	char idPlace[BUFF_SIZE]; // Payload
	int id, idAccount = getIdFromUsername(arrClient[idClient].username);
	int	offset; // Position in arrFavorite
	char data[BUFF_SIZE]; // Contain data save to file
	memset(idPlace, 0, sizeof(idPlace));
	if (strcmp(arrClient[idClient].username, "")) {
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			idPlace[j] = message[i];
			j++;
		}

		cout << "Id place" << idPlace << endl;

		id = atoi(idPlace);

		if ((offset = checkFavorite(id, idAccount)) == -1) {
			memcpy(output, "061", BUFF_SIZE);
			return;
		}
		else {
			char buff[10];
			auto it = arrFavorite.begin() + offset;
			EnterCriticalSection(&critical);
			arrFavorite.erase(it);
			LeaveCriticalSection(&critical);
			// Update Favorite file
			rewriteFavoriteFile();
			memcpy(output, "060", BUFF_SIZE);
			return;
		}
	}
	else {
		memcpy(output, "021", BUFF_SIZE);
	}
}

void getFavorite(char* message, char* output, int idClient) {
	if (strcmp(arrClient[idClient].username, "")) {
		int idAccount = getIdFromUsername(arrClient[idClient].username);
		char username[BUFF_SIZE]; // Store payload
		memset(username, 0, sizeof(username));
		//Get payload
		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}
		cout << "user name: " << username;

		int id_user = getIdFromUsername(username);
		cout << "id" << idAccount << " " << id_user;
		vector<pair<int, int>> listIdPlace;
		for (int i = 0; i < arrFavorite.size(); i++) {
			if (arrFavorite[i].id_account == id_user) {
				listIdPlace.push_back(make_pair(arrFavorite[i].id_place, arrFavorite[i].id_friend));
			}
		}

		cout << "size" << arrFavorite.size() << "size" << listIdPlace.size() << "\n";

		char listPlace[BUFF_SIZE];
		memset(listPlace, 0, sizeof(BUFF_SIZE));
		for (int i = 0; i < listIdPlace.size(); i++) {
			char buff[BUFF_SIZE];
			getPlaceFromId(listIdPlace[i].first, buff);
			strcat_s(listPlace, buff);
			strcat_s(listPlace, " (id: ");
			_itoa(listIdPlace[i].first, buff, 10);
			strcat_s(listPlace, buff);
			strcat_s(listPlace, ")");
			if (listIdPlace[i].second > -1) {
				strcat_s(listPlace, " shared by ");
				getUsernameFromId(listIdPlace[i].second, buff);
				strcat_s(listPlace, buff);
			}

			// Delimiter with next element
			strcat_s(listPlace, "\n");
		}
		if (strlen(listPlace) <= 0) {
			memcpy(output, "051", BUFF_SIZE);
			return;
		}
		else {
			char sBuff[BUFF_SIZE];
			memset(sBuff, 0, sizeof(sBuff));
			strcat_s(sBuff, "070*");
			strcat_s(sBuff, listPlace);
			memcpy(output, sBuff, BUFF_SIZE);
		}

	}
	else {
		memcpy(output, "021", BUFF_SIZE);
	}
}

void storeFavorite(char* message, char* output, int idClient) {
	int idAccount = getIdFromUsername(arrClient[idClient].username);
	if (strcmp(arrClient[idClient].username, "")) {
		char username[BUFF_SIZE]; // Payload
		char listPlace[BUFF_SIZE];
		char path[BUFF_SIZE]; // File path to backup data
		memset(path, 0, sizeof(path));
		memset(listPlace, 0, sizeof(listPlace));
		memset(username, 0, sizeof(username));


		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}

		// Create file path
		strcat_s(path, STORE_FOLDER);
		strcat_s(path, "\\");
		strcat_s(path, username);
		strcat_s(path, ".csv");

		cout << "User name: " << username;

		int id_user = getIdFromUsername(username);
		cout << "Id" << idAccount;
		for (int i = 0; i < arrFavorite.size(); i++) {
			char buff[10];
			if (arrFavorite[i].id_account == id_user) {
				_itoa(id_user, buff, 10);
				strcat_s(listPlace, buff);
				strcat_s(listPlace, ",");

				_itoa(arrFavorite[i].id_place, buff, 10);
				strcat_s(listPlace, buff);
				strcat_s(listPlace, ",");

				_itoa(arrFavorite[i].id_friend, buff, 10);
				strcat_s(listPlace, buff);
				strcat_s(listPlace, "\n"); // New line
			}
		}

		cout << "size" << arrFavorite.size() << " " << listPlace << "\n";

		// Remove last save file and rewrite new
		EnterCriticalSection(&critical);
		remove(path);
		LeaveCriticalSection(&critical);
		SaveData(path, listPlace);
		memcpy(output, "150", sizeof(BUFF_SIZE));
	}
	else {
		memcpy(output, "021", sizeof(BUFF_SIZE));
	}
}

void restoreFavorite(char* message, char* output, int idClient) {
	int idAccount = getIdFromUsername(arrClient[idClient].username);
	if (strcmp(arrClient[idClient].username, "")) {
		char username[BUFF_SIZE]; // Payload
		char listPlace[BUFF_SIZE];
		char path[BUFF_SIZE]; // File path
		memset(path, 0, sizeof(path));
		memset(listPlace, 0, sizeof(listPlace));
		memset(username, 0, sizeof(username));

		int i = 0, j = 0;
		for (i = 0; i < strlen(message) && message[i] != '*'; i++) {

		}
		i++;
		for (; i < strlen(message); i++) {
			username[j] = message[i];
			j++;
		}

		// Create file path
		strcat_s(path, STORE_FOLDER);
		strcat_s(path, "\\");
		strcat_s(path, username);
		strcat_s(path, ".csv");

		cout << "User name: " << username;

		int id_user = getIdFromUsername(username);
		cout << "Id" << idAccount;

		// Store last favorite list
		vector<int> listErase;
		for (int i = 0; i < arrFavorite.size(); i++) {
			if (arrFavorite[i].id_account == id_user) {
				listErase.push_back(i);
				// Remove current favorite places from arrFavorite
				EnterCriticalSection(&critical);
				arrFavorite.erase(arrFavorite.begin() + i);
				LeaveCriticalSection(&critical);
				i--;
			}
		}

		cout << "size" << arrFavorite.size() << "size" << listErase.size() << "\n";

		// Recover new data from file and save
		vector<Favorite> tempList;
		FILE* fp;
		int accountId;
		int placeId, f;

		fp = fopen(path, "r");

		if (fp == NULL) {
			printf("%s file not open!\n", path);
			memcpy(output, "161", sizeof(BUFF_SIZE));
			return;
		}

		//Read each line by fscan
		while (fscanf(fp, "%d,%d,%d\n", &accountId, &placeId, &f) != EOF) {
			tempList.push_back(Favorite(accountId, placeId, f));
		}

		cout << endl;
		fclose(fp); //Close file

		cout << tempList.size() << endl;
		memset(listPlace, 0, sizeof(BUFF_SIZE));

		for (int i = 0; i < tempList.size(); i++) {
			// Add to favorite
			EnterCriticalSection(&critical);
			arrFavorite.push_back(Favorite(tempList[i].id_account, tempList[i].id_place, tempList[i].id_friend));
			LeaveCriticalSection(&critical);
			char buff[BUFF_SIZE];
			getPlaceFromId(tempList[i].id_place, buff);
			strcat_s(listPlace, buff);
			strcat_s(listPlace, " (id: ");
			_itoa(tempList[i].id_place, buff, 10);
			strcat_s(listPlace, buff);
			strcat_s(listPlace, ")");
			if (tempList[i].id_friend > -1) {
				strcat_s(listPlace, " shared by ");
				getUsernameFromId(tempList[i].id_friend, buff);
				strcat_s(listPlace, buff);
			}

			// Delimiter with next element
			strcat_s(listPlace, "\n");
		}

		cout << arrFavorite.size() << endl;
		if (strlen(listPlace) <= 0) {
			memcpy(output, "161", BUFF_SIZE);
			return;
		}
		else {
			char sBuff[BUFF_SIZE];
			memset(sBuff, 0, sizeof(sBuff));
			strcat_s(sBuff, "160*");
			strcat_s(sBuff, listPlace);
			memcpy(output, sBuff, BUFF_SIZE);
			// Update Favorite file
			rewriteFavoriteFile();
		}
	}
	else {
		memcpy(output, "021", BUFF_SIZE);
	}
}

int Send(SOCKET s, char* buff, int size, int flags) {
	int n;

	n = send(s, buff, size, flags);
	if (n == SOCKET_ERROR)
		printf("Error: %\n", WSAGetLastError());

	return n;
}

unsigned __stdcall workerThread(void* param) {

	int index = (int)param;
	int id = index;
	index = index * FD_SETSIZE;

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
				char* temp = strstr(queueBuff[i], ENDING_DELIMITER);
				if (!temp) continue;
				else {
					memset(buff, 0, sizeof(buff));
					int l1 = strlen(queueBuff[i]), l2 = strlen(temp);
					for (int j = 0; j < l1 - l2; j++) {
						buff[j] = queueBuff[i][j];
					}
					buff[l1 - l2] = 0;
					strcpy_s(queueBuff[i], temp + 2);
					printf("Handling buffer:%s\n", buff);

					getHeader(buff, buffHeader);
					printf("header: %s", buffHeader);

					if (!strcmp(buffHeader, "GETFR")) {
						getFriend(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "ADDFR")) {
						addFriend(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "ACCEPT")) {
						acceptFriend(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "DECLINE")) {
						declineFriend(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "SHARE")) {
						share(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "LOGIN")) {
						Login(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "LOGOUT")) {
						Logout(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "SIGNUP")) {
						SignUp(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "GETPLC")) {
						GetPlacesByCategoryId(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "ADDPLC")) {
						AddPlace(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "GETRESF")) {
						getNotificationFriendOfAccount(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "GETCTG")) {
						getListCategory(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "ADDCTG")) {
						AddCategory(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "SAVEPLC")) {
						savePlace(buff, sbuff, i, -1);
					}
					else if (!strcmp(buffHeader, "DELETE")) {
						deletePlace(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "GETFAV")) {
						getFavorite(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "STORE")) {
						storeFavorite(buff, sbuff, i);
					}
					else if (!strcmp(buffHeader, "RESTORE")) {
						restoreFavorite(buff, sbuff, i);
					}

					printf("\nSend to client: ,%s,\n", sbuff);
					Send(arrClient[i].s, sbuff, strlen(sbuff), 0);
				}
			}
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	const char* SERVER_ADDR = _strdup(argv[1]);
	const int SERVER_PORT = atoi(argv[2]);
	if (!_mkdir(STORE_FOLDER)) {
		cout << "Store folder is created" << endl;
	}
	else {
		cout << "Store folder has exited" << endl;
	}
	ReadAccountFile();
	ReadPlaceFile();
	ReadCategoryFile();
	ReadFriendFile();
	ReadFavoriteFile();
	ReadNotificationFile();
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
	if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)))
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
		connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
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
					if (cnt * FD_SETSIZE <= i) {
						printf("____________________________\nCreate new thread \n");

						_beginthreadex(0, 0, workerThread, (void*)cnt, 0, 0); //start thread
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