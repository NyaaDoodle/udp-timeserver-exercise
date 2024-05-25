#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>

class TimeClient {
public:
	void run();
private:
	WSAData wsaData;
	SOCKET connSocket;
	sockaddr_in server;
	int bytesSent, bytesRecv;
	char sendBuff[255], recvBuff[255];
	void initialize_dll();
	void make_socket();
	void set_destination();
	void send_string_message();
	void expect_server_response();
	void close_client();
};