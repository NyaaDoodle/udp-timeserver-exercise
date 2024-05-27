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

	// Internal client operations
	void initialize_dll();
	void make_socket();
	void set_destination();
	void send_string_message(const std::string message, const bool is_silent = false);
	void expect_server_response(const bool is_silent = false);
	void close_client();
	void main_menu();
	// Time related actions
	void get_time();
	void get_time_no_date();
	void get_unix_time();
	void get_time_no_date_no_seconds();
	void get_year();
	void get_month_day();
	void get_seconds_from_month_beginning();
	void get_week_of_year();
	void get_if_daylight_savings();
	void get_time_no_date_specified_city();
	// Client-server connection related actions
	void measure_delay_estimate();
	void measure_rtt();
	void measure_time_lap();
	// Helper functions
	std::string select_city();
	std::string string_tolower(const std::string& str);
};