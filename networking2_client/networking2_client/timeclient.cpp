#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "timeclient.h"
#include <iostream>
#include <string.h>
#include <algorithm>
#include <cctype>

#define TIME_PORT	27015

void TimeClient::run() {
	initialize_dll();
	make_socket();
	set_destination();
	std::cout << "Time client is set up." << std::endl;
	while (!to_exit) {
		main_menu();
	}
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
void TimeClient::send_string_message(const std::string message, const bool is_silent) {
	bytesSent = bytesRecv = 0;
	strcpy(sendBuff, message.c_str());
	bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent) {
		std::cout << "Time Client: Error at sendto(): " << WSAGetLastError() << std::endl;
		closesocket(connSocket);
		WSACleanup();
		exit(-3);
	}
	if (!is_silent) {
		std::cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}
}
void TimeClient::expect_server_response(const bool is_silent) {
	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv) {
		std::cout << "Time Client: Error at recv(): " << WSAGetLastError() << std::endl;
		closesocket(connSocket);
		WSACleanup();
		exit(-4);
	}
	recvBuff[bytesRecv] = '\0';
	if (!is_silent) {
		std::cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
	}
}
void TimeClient::close_client() {
	std::cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);
	WSACleanup();
}
void TimeClient::main_menu() {
	std::string message = "";
	int user_input;
	std::cout << "Enter a number to select your option:\n";
	std::cout << "(1) Get time\n";
	std::cout << "(2) Get time without date\n";
	std::cout << "(3) Get time since epoch (unix time)\n";
	std::cout << "(4) Get client-to-server delay estimation\n";
	std::cout << "(5) Measure client-to-server RTT\n";
	std::cout << "(6) Get time without date or seconds\n";
	std::cout << "(7) Get year\n";
	std::cout << "(8) Get month and day\n";
	std::cout << "(9) Get seconds since the beginning of this month\n";
	std::cout << "(10) Get week number of the current year\n";
	std::cout << "(11) Get if daylight savings is set currently\n";
	std::cout << "(12) Get time without date in city\n";
	std::cout << "(13) Measure time lap\n";
	std::cout << "(0) Exit client\n";
	std::cin >> user_input;
	std::cin.ignore(BUFFER_SIZE, '\n');
	switch (user_input) {
	case 0:
		to_exit = true;
		break;
	case 1:
		send_string_message("time");
		expect_server_response();
		break;
	case 2:
		send_string_message("time no date");
		expect_server_response();
		break;
	case 3:
		send_string_message("time unix");
		expect_server_response();
		break;
	case 4:
		// TODO
		send_string_message("time");
		expect_server_response();
		break;
	case 5:
		// TODO
		send_string_message("time");
		expect_server_response();
		break;
	case 6:
		send_string_message("time no date no seconds");
		expect_server_response();
		break;
	case 7:
		send_string_message("time only year");
		expect_server_response();
		break;
	case 8:
		send_string_message("time only month day");
		expect_server_response();
		break;
	case 9:
		send_string_message("time only seconds relativeto currmonth");
		expect_server_response();
		break;
	case 10:
		send_string_message("time only week relativeto curryear");
		expect_server_response();
		break;
	case 11:
		send_string_message("time isdst");
		expect_server_response();
		break;
	case 12:
		message = "time no date at ";
		message += select_city();
		send_string_message(message);
		expect_server_response();
		break;
	case 13:
		// TODO
		send_string_message("time lap");
		expect_server_response();
		if (strcmp(recvBuff, "Lap timer started, send second request") == 0) {
			send_string_message("time lap");
			expect_server_response();
		}
		break;
	default:
		break;
	}
	std::cout << std::endl;
}
std::string TimeClient::select_city() {
	std::string userInput;
	std::cout << "Enter the name of the desired city:\n";
	std::cout << "* Doha, Qatar\n";
	std::cout << "* Prague, Czech Republic\n";
	std::cout << "* New York, United States of America\n";
	std::cout << "* Berlin, Germany\n";
	std::cout << "Any city specified that is not on the list, UTC time will be returned.\n";
	std::getline(std::cin, userInput);
	return string_tolower(userInput);
}

std::string TimeClient::string_tolower(const std::string& str) {
	std::string out_str = str;
	std::transform(out_str.begin(), out_str.end(), out_str.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return out_str;
}
