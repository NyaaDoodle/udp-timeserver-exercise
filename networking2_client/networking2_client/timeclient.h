#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string>

class TimeClient {
public:
	void run();
private:
	WSAData wsaData;
	SOCKET connSocket;
	sockaddr_in server;
	int bytesSent, bytesRecv;
	char sendBuff[255], recvBuff[255];
	bool to_exit = false;
	void initialize_dll();
	void make_socket();
	void set_destination();
	void send_string_message(const std::string message, const bool is_silent = false);
	void expect_server_response(const bool is_silent = false);
	void close_client();
	void main_menu();
};