#define _CRT_SECURE_NO_WARNINGS
#include "timeserver.h"
#include <iostream>
#include <string.h>
#include <time.h>
#include <Windows.h>

#define TIME_PORT	27015

TimeServer::TimeServer() {
	initStringToServerAction();
	initStringToUTCOffset();
}

int TimeServer::run() {
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		std::cout << "Time Server: Error at WSAStartup()\n";
		return - 1;
	}
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket) {
		std::cout << "Time Server: Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return -2;
	}
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(TIME_PORT);
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService))) {
		std::cout << "Time Server: Error at bind(): " << WSAGetLastError() << std::endl;
		closesocket(m_socket);
		WSACleanup();
		return -3;
	}
	client_addr_len = sizeof(client_addr);
	bytesSent = bytesRecv = 0;
	std::cout << "Time Server: Wait for clients' requests.\n";
	while (true) {
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv) {
			std::cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << std::endl;
			closesocket(m_socket);
			WSACleanup();
			return -4;
		}
		recvBuff[bytesRecv] = '\0';
		std::cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
		strcpy(sendBuff, select_action(recvBuff).c_str());
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent) {
			std::cout << "Time Server: Error at sendto(): " << WSAGetLastError() << std::endl;
			closesocket(m_socket);
			WSACleanup();
			return -5;
		}
		std::cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}
	std::cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
	return 0;
}

std::string TimeServer::select_action(const char* recv_message) {
	return get_client_server_tick_count();
}
// Time related actions
std::string TimeServer::get_time() {
	time(&timer);
	strcpy(buf, ctime(&timer));
	buf[strlen(buf) - 1] = '\0';
	return buf;
}
std::string TimeServer::get_time_no_date() {
	return strftime_by_format("%T");
}
std::string TimeServer::get_unix_time() {
	sprintf(buf, "%d", (int)time(&timer));
	return buf;
}
std::string TimeServer::get_time_no_date_no_seconds() {
	return strftime_by_format("%H:%M");
}
std::string TimeServer::get_year() {
	return strftime_by_format("%Y");
}
std::string TimeServer::get_month_day() {
	return strftime_by_format("%d/%m");
}
std::string TimeServer::get_seconds_from_month_beginning() {
	constexpr int SECONDS_PER_MINUTE = 60;
	constexpr int SECONDS_PER_HOUR = SECONDS_PER_MINUTE * 60;
	constexpr int SECONDS_PER_DAY = SECONDS_PER_HOUR * 24;
	int seconds_from_beginning_of_month = 0;
	time(&timer);
	timeinfo = localtime(&timer);
	seconds_from_beginning_of_month += (timeinfo->tm_mday - 1) * SECONDS_PER_DAY;
	seconds_from_beginning_of_month += (timeinfo->tm_hour) * SECONDS_PER_HOUR;
	seconds_from_beginning_of_month += (timeinfo->tm_min) * SECONDS_PER_MINUTE;
	seconds_from_beginning_of_month += timeinfo->tm_sec;
	sprintf(buf, "%d", seconds_from_beginning_of_month);
	return buf;
}
std::string TimeServer::get_week_of_year() {
	return strftime_by_format("%V");
}
std::string TimeServer::get_if_daylight_savings() {
	time(&timer);
	timeinfo = localtime(&timer);
	sprintf(buf, "%d", timeinfo->tm_isdst);
	return buf;
}
std::string TimeServer::get_time_no_date_specified_city(const char* city) {
	std::string message = "";
	int utc_offset;
	const auto itr = stringToUTCOffset.find(city);
	if (itr != stringToUTCOffset.end()) {
		utc_offset = itr->second;
	}
	else {
		utc_offset = 0;
		message = "Specified city not supported. UTC time: ";
	}
	time(&timer);
	timeinfo = gmtime(&timer);
	sprintf(buf, "%2d:%02d:%02d", (timeinfo->tm_hour + utc_offset), (timeinfo->tm_min), (timeinfo->tm_sec));
	message += buf;
	return message;
}
// Client-server connection related actions
std::string TimeServer::get_client_server_tick_count() {
	sprintf(buf, "%d", GetTickCount());
	return buf;
}
std::string TimeServer::measure_time_lap() {
	constexpr int SECONDS_IN_THREE_MINUTES = 180;
	strcpy(buf, "Lap timer started, send second request");
	// send message
	time(&timer);
	time_t currtime;
	time_t endtime = timer + SECONDS_IN_THREE_MINUTES;
	while (currtime = time(&timer) < endtime) {
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv) {
			std::cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << std::endl;
			closesocket(m_socket);
			WSACleanup();
			return "-4";
		}
		recvBuff[bytesRecv] = '\0';
		if (strcmp(recvBuff, "time lap")) {
			sprintf(buf, "Time elapsed: %dsec", (int)currtime);
			// send message
		}
	}
	return "";
}
// Helper functions
std::string TimeServer::strftime_by_format(const char* format) {
	time(&timer);
	timeinfo = localtime(&timer);
	strftime(buf, BUFFER_SIZE, format, timeinfo);
	return buf;
}
void TimeServer::initStringToServerAction() {
	stringToServerAction["time"] = ServerAction::GetTime;
	stringToServerAction["time no date"] = ServerAction::GetTimeWithoutDate;
	stringToServerAction["time unix"] = ServerAction::GetTimeSinceEpoch;
	stringToServerAction["time bench"] = ServerAction::GetClientToServerTickCount;
	stringToServerAction["time no date no seconds"] = ServerAction::GetTimeWithoutDateOrSeconds;
	stringToServerAction["time only year"] = ServerAction::GetYear;
	stringToServerAction["time only month only day"] = ServerAction::GetMonthAndDay;
	stringToServerAction["time only seconds realtiveto currmonth"] = ServerAction::GetSecondsSinceBeginningOfMonth;
	stringToServerAction["time only week relativeto curryear"] = ServerAction::GetWeekOfYear;
	stringToServerAction["time isdst"] = ServerAction::GetDaylightSavings;
	stringToServerAction["time no date at"] = ServerAction::GetTimeWithoutDateInCity;
	stringToServerAction["time lap"] = ServerAction::MeasureTimeLap;
}
void TimeServer::initStringToUTCOffset() {
	stringToUTCOffset["doha"] = 3;
	stringToUTCOffset["prague"] = 2;
	stringToUTCOffset["new york"] = -4;
	stringToUTCOffset["berlin"] = 2;
}