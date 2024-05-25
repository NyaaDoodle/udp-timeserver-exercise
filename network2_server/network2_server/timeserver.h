#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <map>

#define BUFFER_SIZE 256

enum class ServerAction {
	GetTime, GetTimeWithoutDate, GetTimeSinceEpoch, GetClientToServerTickCount,
	GetTimeWithoutDateOrSeconds, GetYear, GetMonthAndDay, GetSecondsSinceBeginningOfMonth,
	GetWeekOfYear, GetDaylightSavings, GetTimeWithoutDateInCity, MeasureTimeLap
};

enum class City { Doha, Prague, NewYork, Berlin };

class TimeServer {
public:
	TimeServer();
	int run();
private:
	WSAData wsaData;
	SOCKET m_socket;
	sockaddr_in serverService;
	sockaddr client_addr;
	int client_addr_len, bytesSent, bytesRecv;
	char sendBuff[BUFFER_SIZE], recvBuff[BUFFER_SIZE];
	char buf[BUFFER_SIZE];
	time_t timer;
	struct tm* timeinfo;
	std::map<std::string, ServerAction> stringToServerAction;
	std::map<std::string, int> stringToUTCOffset;

	std::string select_action(const char *recv_message);
	// Time related actions
	std::string get_time();
	std::string get_time_no_date();
	std::string get_unix_time();
	std::string get_time_no_date_no_seconds();
	std::string get_year();
	std::string get_month_day();
	std::string get_seconds_from_month_beginning();
	std::string get_week_of_year();
	std::string get_if_daylight_savings();
	std::string get_time_no_date_specified_city(const char* city);
	// Client-server connection related actions
	std::string get_client_server_tick_count();
	std::string measure_time_lap();
	// Helper functions
	std::string strftime_by_format(const char* format);
	void initStringToServerAction();
	void initStringToUTCOffset();
};