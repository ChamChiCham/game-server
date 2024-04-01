#pragma once
#include <string>
#include <WS2tcpip.h>
#include <queue>
#pragma comment(lib, "WS2_32.LIB")

void CALLBACK send_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK recv_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

#include "Define.h"

void print_error(const char* msg, int err_no);

struct EXP_OVER
{
	WSAOVERLAPPED over;
	WSABUF wsabuf[1];
	char buf[NETWORK_BUFFER_SIZE];
	EXP_OVER()
	{
		ZeroMemory(&over, sizeof(over));
		wsabuf[0].buf = buf;
		wsabuf[0].len = NETWORK_BUFFER_SIZE;
	}
};

class CNetworkMgr
{
private:
	std::string server_address;

	SOCKET server_socket;
	EXP_OVER send_exp;
	EXP_OVER recv_exp;

	int id{-1};

	std::queue<std::vector<char>> message_queue;

public:
	~CNetworkMgr();

	void init();
	void setServerAddress();

	void doSend(std::vector<char>);
	void doRecv();

	void pushMessage();
	const std::vector<char> popMessage();

	void setId();

};

extern CNetworkMgr network_mgr;