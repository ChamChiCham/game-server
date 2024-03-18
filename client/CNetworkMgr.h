#pragma once
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.LIB")

void print_error(const char* msg, int err_no);

class CNetworkMgr
{
private:
	std::string server_address;
	SOCKET server_socket;

public:
	void init();
	std::string sendMessage(std::string_view);
	void setServerAddress();

	~CNetworkMgr();
};