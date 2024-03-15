#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")
const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

int main()
{
	// Level 0: init
	// Winsock 초기화
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	
	// Level 1: Socket
	// 소켓 생성
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);

	// Level 2: Bind
	// 바인딩
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	// Level 3: Listen
	// 대기열 설정
	listen(s_socket, SOMAXCONN);

	// Level 4: Accept
	// 클라이언트 연결 대기
	INT addr_size = sizeof(server_addr);
	SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
	
	// Level 5: Recv/Send
	// 클라이언트와의 통신
	for (;;) {

		// 클라이언트로부터 메시지 받기
		char recv_buf[BUFSIZE];
		WSABUF mybuf;
		mybuf.buf = recv_buf;  mybuf.len = BUFSIZE;
		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(c_socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);

		// 클라이언트로부터 받은 메시지 출력
		cout << "Client Sent [" << recv_byte << "bytes] : " << recv_buf << endl;

		// 클라이언트에게 메시지 보내기
		DWORD sent_byte;
		mybuf.len = recv_byte;
		WSASend(c_socket, &mybuf, 1, &sent_byte, 0, 0, 0);
	}

	// Level 6: Close
	// 소켓 닫기
	WSACleanup();
}
