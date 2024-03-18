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
	// Level 0: lnit
	// 한글 설정
	std::wcout.imbue(std::locale("korean"));

	// Winsock 초기화
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata);

	// Level 1:	Socket
	// 소켓 생성
	SOCKET server_s{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0) };

	// Level 2:	Connect
	// 서버 주소 설정
	SOCKADDR_IN server_a;
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);
	inet_pton(AF_INET, ADDR, &server_a.sin_addr);
	connect(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));


	// Level 3:	Send/Recv
	// 메시지 전송
	while (true) {

		// Send
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

		// Recv
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
	// Level 4:	Close
	// 소켓 닫기
	closesocket(server_s);
	WSACleanup();
}