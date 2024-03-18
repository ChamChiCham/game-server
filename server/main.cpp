#include <iostream>
#include <WS2tcpip.h>
#include <utility>
#include <string>
#pragma comment (lib, "WS2_32.LIB")

constexpr short PORT = 4'000;
constexpr char ADDR[] = "127.0.0.1";
constexpr int BUFSIZE = 256;

std::pair<int, int> queen_pos;
char queen_status;


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

const std::string handle_data(const char* recv, const DWORD length)
{
	std::string send;
	if (length < 2);
	else if (recv[0] == 'U') {
		if (recv[1] == 'U') {
			queen_status &= ~0x01;
		}
		else if (recv[1] == 'D') {
			queen_status &= ~0x02;
		}
		else if (recv[1] == 'L') {
			queen_status &= ~0x04;
		}
		else if (recv[1] == 'R') {
			queen_status &= ~0x08;
		}
	}
	else if (recv[0] == 'D') {
		if (recv[1] == 'U' && !(queen_status & 0x01)) {
			queen_status |= 0x01;
			if (queen_pos.second > 0) {
				queen_pos.second--;
				send = "MU";
			}
		}
		else if (recv[1] == 'D' && !(queen_status & 0x02)) {
			queen_status |= 0x02;
			if (queen_pos.second < 7) {
				queen_pos.second++;
				send = "MD";
			}
		}
		else if (recv[1] == 'L' && !(queen_status & 0x04)) {
			queen_status |= 0x01;
			if (queen_pos.first > 0) {
				queen_pos.first--;
				send = "ML";
			}
		}
		else if (recv[1] == 'R' && !(queen_status & 0x08)) {
			queen_status |= 0x08;
			if (queen_pos.first < 7) {
				queen_pos.first++;
				send = "MR";
			}
		}
	}
	return send;
}

int main()
{
	// Level 0: init
	// 한글 설정
	std::wcout.imbue(std::locale("korean"));

	// Winsock 초기화
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata);
	std::cout << "Success to Start up Window Socket." << std::endl;


	// Level 1: Socket
	// 소켓 생성
	SOCKET server_s{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0) };
	std::cout << "Success to create Socket." << std::endl;

	// Level 2: Bind
	// 바인딩
	SOCKADDR_IN server_a;
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);
	server_a.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));

	// Level 3: Listen
	// 대기열 설정
	listen(server_s, SOMAXCONN);

	// Level 4: Accept
	// 클라이언트 연결 대기
	int addr_size = sizeof(server_a);
	std::cout << "Waiting for client..." << std::endl;
	SOCKET client_s = WSAAccept(server_s, reinterpret_cast<sockaddr*>(&server_a), &addr_size, nullptr, 0);
	std::cout << "Success to connect client." << std::endl;

	// Level 5: Recv/Send
	// 클라이언트와의 통신
	while (true) {

		// recv
		char buf[BUFSIZE]{};
		WSABUF wsabuf[1];
		wsabuf[0].buf = buf;
		wsabuf[0].len = static_cast<int>(BUFSIZE);
		DWORD recv_size;
		DWORD recv_flag{ 0 };
		std::cout << "QUEEN: " << queen_pos.first << ", " << queen_pos.second << std::endl;
		std::cout << "Waiting for data..." << std::endl;
		int res = WSARecv(client_s, &wsabuf[0], 1, &recv_size, &recv_flag, nullptr, nullptr);
		if (0 != res) {
			print_error("WSARecv", WSAGetLastError());
		}
		if (wsabuf[0].len == 1) {
			std::cout << "End program." << std::endl;
			break;
		}
		std::cout << "Success to recv data." << std::endl;
		auto result = handle_data(buf, recv_size);
		// send
		strcpy_s(buf, sizeof(buf), result.c_str());
		wsabuf[0].buf = buf;
		wsabuf[0].len = static_cast<int>(strlen(buf) + 1);
		DWORD sent_size;
		WSASend(client_s, &wsabuf[0], 1, &sent_size, 0, 0, 0);
		std::cout << "Success to send data." << std::endl;

	}

	// Level 6: Close
	// 소켓 닫기
	closesocket(server_s);
	WSACleanup();
}