//#include <iostream>
//#include <WS2tcpip.h>
//using namespace std;
//#pragma comment (lib, "WS2_32.LIB")
//const short SERVER_PORT = 4000;
//const int BUFSIZE = 256;
//
//int main()
//{
//	// Level 0: init
//	// Winsock 초기화
//	WSADATA WSAData;
//	WSAStartup(MAKEWORD(2, 0), &WSAData);
//	
//	// Level 1: Socket
//	// 소켓 생성
//	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
//
//	// Level 2: Bind
//	// 바인딩
//	SOCKADDR_IN server_addr;
//	ZeroMemory(&server_addr, sizeof(server_addr));
//	server_addr.sin_family = AF_INET;
//	server_addr.sin_port = htons(SERVER_PORT);
//	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
//
//	// Level 3: Listen
//	// 대기열 설정
//	listen(s_socket, SOMAXCONN);
//
//	// Level 4: Accept
//	// 클라이언트 연결 대기
//	INT addr_size = sizeof(server_addr);
//	SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
//	
//	// Level 5: Recv/Send
//	// 클라이언트와의 통신
//	for (;;) {
//
//		// 클라이언트로부터 메시지 받기
//		char recv_buf[BUFSIZE];
//		WSABUF mybuf;
//		mybuf.buf = recv_buf;  mybuf.len = BUFSIZE;
//		DWORD recv_byte;
//		DWORD recv_flag = 0;
//		WSARecv(c_socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
//
//		// 클라이언트로부터 받은 메시지 출력
//		cout << "Client Sent [" << recv_byte << "bytes] : " << recv_buf << endl;
//
//		// 클라이언트에게 메시지 보내기
//		DWORD sent_byte;
//		mybuf.len = recv_byte;
//		WSASend(c_socket, &mybuf, 1, &sent_byte, 0, 0, 0);
//	}
//
//	// Level 6: Close
//	// 소켓 닫기
//	WSACleanup();
//}

#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "WS2_32.LIB")

constexpr short PORT = 4'000;
constexpr char ADDR[] = "127.0.0.1";
constexpr int BUFSIZE = 256;

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

int main()
{
	std::wcout.imbue(std::locale("korean"));

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata);

	SOCKET server_s{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0) };

	SOCKADDR_IN server_a;
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);
	server_a.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));

	listen(server_s, SOMAXCONN);

	int addr_size = sizeof(server_a);
	SOCKET client_s = WSAAccept(server_s, reinterpret_cast<sockaddr*>(&server_a), &addr_size, nullptr, 0);
	while (true) {

		char buf[BUFSIZE];
		WSABUF wsabuf[1];
		wsabuf[0].buf = buf;
		wsabuf[0].len = static_cast<int>(BUFSIZE);
		DWORD recv_size;
		DWORD recv_flag{ 0 };
		int res = WSARecv(client_s, &wsabuf[0], 1, &recv_size, &recv_flag, nullptr, nullptr);
		if (0 != res) {
			print_error("WSARecv", WSAGetLastError());
		}
		if (wsabuf[0].len == 1) {
			break;
		}

		for (DWORD i{ 0 }; i < recv_size; ++i) {
			std::cout << buf[i];
		}
		std::cout << std::endl;

		wsabuf[0].buf = buf;
		wsabuf[0].len = static_cast<int>(strlen(buf) + 1);
		DWORD sent_size;
		WSASend(client_s, &wsabuf[0], 1, &sent_size, 0, 0, 0);

	}
	closesocket(server_s);
	WSACleanup();
}