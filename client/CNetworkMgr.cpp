#include <iostream>

#include "CNetworkMgr.h"
#include "Define.h"

CNetworkMgr network_mgr;

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

	// Level 1:	Overlapped Socket
	// 소켓 생성
	server_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
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

	// Level 3: Overlapped Recv
	// 서버로부터 recv 받기

	doRecv();

}

// ----
// send
// ----


// send to server
void CNetworkMgr::doSend(std::string_view msg)
{
	strcpy_s(send_exp.buf, sizeof(send_exp.buf), msg.data());
	send_exp.wsabuf[0].buf = send_exp.buf;
	send_exp.wsabuf[0].len = static_cast<ULONG>(strlen(send_exp.buf)) + 1;

	ZeroMemory(&send_exp.over, sizeof(send_exp.over));
	WSASend(server_socket, send_exp.wsabuf, 1, nullptr, 0, &send_exp.over, send_callback);
}

// send 완료시 callback 함수.
void CALLBACK send_callback(DWORD err, DWORD sent_size,
	LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	if (0 != err) {
		print_error("WSASend", WSAGetLastError());
	}
}


// ----
// recv
// ----

// recv function
void CNetworkMgr::doRecv()
{
	DWORD recv_flag = 0;
	recv_exp.wsabuf[0].buf = recv_exp.buf;
	recv_exp.wsabuf[0].len = NETWORK_BUFFER_SIZE;
	ZeroMemory(&recv_exp.over, sizeof(recv_exp.over));
	int res = WSARecv(server_socket, recv_exp.wsabuf, 1, nullptr, &recv_flag, &recv_exp.over, recv_callback);

	if (0 != res) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			print_error("WSARecv", WSAGetLastError());
	}
}


// recv 완료 후 실행되는 callback
void CALLBACK recv_callback(DWORD err, DWORD recv_size,
	LPWSAOVERLAPPED pover, DWORD recv_flag)
{
	if (0 != err) {
		print_error("WSARecv", WSAGetLastError());
	}

	// --
	// recv process
	// --
 

	// push message
	network_mgr.pushMessage();

	// repeat recv
	network_mgr.doRecv();
}

// 메시지를 메시지 큐에 전달
void CNetworkMgr::pushMessage()
{
	message_queue.push(recv_exp.buf);
}

// 메시지 window에 전달
char* CNetworkMgr::popMessage()
{
	if (message_queue.empty()) {
		return nullptr;
	}
	else {

		char* result{ message_queue.front() };
		message_queue.pop();
		return result;
	}
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
