//#include <iostream>
//#include <WS2tcpip.h>
//using namespace std;
//#pragma comment (lib, "WS2_32.LIB")
//const char* SERVER_ADDR = "127.0.0.1";
//const short SERVER_PORT = 4000;
//const int BUFSIZE = 256;
//int main()
//{
//	// Level 0: lnit
//	// 한글 설정
//	wcout.imbue(locale("korean"));
//
//	// Winsock 초기화
//	WSADATA WSAData;
//	WSAStartup(MAKEWORD(2, 0), &WSAData);
//
//	// Level 1:	Socket
//	// 소켓 생성
//	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
//
//	// Level 2:	Connect
//	// 서버 주소 설정
//	SOCKADDR_IN server_addr;
//	ZeroMemory(&server_addr, sizeof(server_addr));
//	server_addr.sin_family = AF_INET;
//	// htons(): setting little endian, big endian
//	server_addr.sin_port = htons(SERVER_PORT);
//	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
//
//	// 서버에 연결
//	connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
//	
//	// Level 3:	Send/Recv
//	// 메시지 전송
//	for (;;) {
//
//		// Send
//		char buf[BUFSIZE];
//		cout << "Enter Message : ";  cin.getline(buf, BUFSIZE);
//		DWORD sent_byte;
//		WSABUF mybuf;
//		mybuf.buf = buf; mybuf.len = static_cast<ULONG>(strlen(buf)) + 1;
//		WSASend(234, &mybuf, 1, &sent_byte, 0, 0, 0);
//
//		// Recv
//		char recv_buf[BUFSIZE];
//		WSABUF mybuf_r;
//		mybuf_r.buf = recv_buf; mybuf_r.len = BUFSIZE;
//		DWORD recv_byte;
//		DWORD recv_flag = 0;
//		WSARecv(s_socket, &mybuf_r, 1, &recv_byte, &recv_flag, 0, 0);
//
//		cout << "Server Sent [" << recv_byte << "bytes] : " << recv_buf << endl;
//	}
//
//
//	// Level 4:	Close
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
	inet_pton(AF_INET, ADDR, &server_a.sin_addr);
	connect(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));
	while (true) {


		char buf[BUFSIZE];
		std::cout << "Enter Message: " << std::endl;
		std::cin.getline(buf, BUFSIZE);
		WSABUF wsabuf[1];
		wsabuf[0].buf = buf;
		wsabuf[0].len = static_cast<int>(strlen(buf) + 1);
		if (wsabuf[0].len == 1) {
			break;
		}
		DWORD sent_size;
		WSASend(server_s, &wsabuf[0], 1, &sent_size, 0, 0, 0);

		wsabuf[0].buf = buf;
		wsabuf[0].len = static_cast<int>(BUFSIZE);
		DWORD recv_size;
		DWORD recv_flag{ 0 };
		int res = WSARecv(server_s, &wsabuf[0], 1, &recv_size, &recv_flag, nullptr, nullptr);
		if (0 != res) {
			print_error("WSARecv", WSAGetLastError());
		}
		for (DWORD i{ 0 }; i < recv_size; ++i) {
			std::cout << buf[i];
		}
		std::cout << std::endl;
		
	}
	closesocket(server_s);
	WSACleanup();
}