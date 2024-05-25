#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "timeclient.h"
#include <iostream>
#include <string.h>

#define TIME_PORT	27015

void TimeClient::run() {
	initialize_dll();
	make_socket();
	set_destination();
	send_string_message();
	expect_server_response();
	close_client();
}
void TimeClient::initialize_dll() {
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		std::cout << "Time Client: Error at WSAStartup()\n";
		exit(-1);
	}
}
void TimeClient::make_socket() {
	connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket) {
		std::cout << "Time Client: Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(-2);
	}
}
void TimeClient::set_destination() {
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);
}
void TimeClient::send_string_message() {
	bytesSent = bytesRecv = 0;
	strcpy(sendBuff, "What's the time?");
	bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent) {
		std::cout << "Time Client: Error at sendto(): " << WSAGetLastError() << std::endl;
		closesocket(connSocket);
		WSACleanup();
		exit(-3);
	}
	std::cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
}
void TimeClient::expect_server_response() {
	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv) {
		std::cout << "Time Client: Error at recv(): " << WSAGetLastError() << std::endl;
		closesocket(connSocket);
		WSACleanup();
		exit(-4);
	}
	recvBuff[bytesRecv] = '\0';
	std::cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
}
void TimeClient::close_client() {
	std::cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);
	WSACleanup();
}