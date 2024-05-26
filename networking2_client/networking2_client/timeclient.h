#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string>

#define BUFFER_SIZE 256

class TimeClient {
public:
	void run();
private:
	WSAData wsaData;
	SOCKET connSocket;
	sockaddr_in server;
	int bytesSent, bytesRecv;
	char sendBuff[BUFFER_SIZE], recvBuff[BUFFER_SIZE];
	bool to_exit = false;
	void initialize_dll();
	void make_socket();
	void set_destination();
	void send_string_message(const std::string message, const bool is_silent = false);
	void expect_server_response(const bool is_silent = false);
	void close_client();
	void main_menu();
	std::string select_city();
	std::string string_tolower(const std::string& str);
};