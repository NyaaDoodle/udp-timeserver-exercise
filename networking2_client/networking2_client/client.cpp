#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h> 
#include <string.h>
#pragma comment(lib, "Ws2_32.lib")

#define TIME_PORT	27015

int main(void) {
	WSAData wsaData;
	SOCKET connSocket;
	sockaddr_in server;
	int bytesSent, bytesRecv;
	char sendBuff[255], recvBuff[255];
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		std::cout << "Time Client: Error at WSAStartup()\n";
		return -1;
	}
	connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket) {
		std::cout << "Time Client: Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return -2;
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);
	bytesSent = bytesRecv = 0;
	strcpy(sendBuff, "What's the time?");
	bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent) {
		std::cout << "Time Client: Error at sendto(): " << WSAGetLastError() << std::endl;
		closesocket(connSocket);
		WSACleanup();
		return -3;
	}
	std::cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv) {
		std::cout << "Time Client: Error at recv(): " << WSAGetLastError() << std::endl;
		closesocket(connSocket);
		WSACleanup();
		return -4;
	}
	recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	std::cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
	std::cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);
	return 0;
}