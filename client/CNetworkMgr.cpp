#include <iostream>

#include "CNetworkMgr.h"
#include "Define.h"

void print_error(const char* msg, int err_no)
{
	WCHAR* msg_buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L"에러: " << msg_buf << std::endl;
	while (true);
	LocalFree(msg_buf);
}


void CNetworkMgr::init()
{
	if (server_address.empty()) {
		std::cout << "server isn't set." << std::endl;
		return;
	}

	// Level 0: lnit
	// 한글 설정
	std::wcout.imbue(std::locale("korean"));

	// Winsock 초기화
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata);
	std::cout << "Success to Start up Window Socket." << std::endl;

	// Level 1:	Socket
	// 소켓 생성
	server_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	std::cout << "Success to create Socket." << std::endl;

	// Level 2:	Connect
	// 서버 주소 설정
	SOCKADDR_IN server_a;
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(NETWORK_PORT);
	inet_pton(AF_INET, server_address.c_str(), &server_a.sin_addr);
	std::cout << "Connecting Server..." << std::endl;
	connect(server_socket, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));
	std::cout << "Success to connect Server." << std::endl;
}

std::string CNetworkMgr::sendMessage(std::string_view msg)
{
	// Send
	char buf[NETWORK_BUFFER_SIZE];
	strcpy_s(buf, sizeof(buf), msg.data());
	WSABUF wsabuf[1];
	wsabuf[0].buf = buf;
	wsabuf[0].len = static_cast<ULONG>(strlen(buf) + 1);
	if (wsabuf[0].len == 1) {
		return std::string{ "Q" };
	}
	DWORD sent_size;
	WSASend(server_socket, &wsabuf[0], 1, &sent_size, 0, 0, 0);

	// Recv
	wsabuf[0].buf = buf;
	wsabuf[0].len = static_cast<ULONG>(NETWORK_BUFFER_SIZE);
	DWORD recv_size;
	DWORD recv_flag{ 0 };
	int res = WSARecv(server_socket, &wsabuf[0], 1, &recv_size, &recv_flag, nullptr, nullptr);
	if (0 != res) {
		print_error("WSARecv", WSAGetLastError());
	}

	return std::string{buf};
}

void CNetworkMgr::setServerAddress()
{
	std::cout << "Input server address: " << std::endl;
	std::cin >> server_address;
}

CNetworkMgr::~CNetworkMgr()
{
	// Level 4:	Close
	// 소켓 닫기
	closesocket(server_socket);
	WSACleanup();
}
