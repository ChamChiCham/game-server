#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")
const char* SERVER_ADDR = "127.0.0.1";
const short SERVER_PORT = 4000;
const int BUFSIZE = 256;
int main()
{
	// Level 0: lnit
	// 한글 설정
	wcout.imbue(locale("korean"));

	// Winsock 초기화
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	// Level 1:	Socket
	// 소켓 생성
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);

	// Level 2:	Connect
	// 서버 주소 설정
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	// 서버에 연결
	connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	
	// Level 3:	Recv/Send
	// 메시지 전송
	for (;;) {
		char buf[BUFSIZE];
		cout << "Enter Message : ";  cin.getline(buf, BUFSIZE);
		DWORD sent_byte;
		WSABUF mybuf;
		mybuf.buf = buf; mybuf.len = static_cast<ULONG>(strlen(buf)) + 1;
		WSASend(234, &mybuf, 1, &sent_byte, 0, 0, 0);

		char recv_buf[BUFSIZE];
		WSABUF mybuf_r;
		mybuf_r.buf = recv_buf; mybuf_r.len = BUFSIZE;
		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(s_socket, &mybuf_r, 1, &recv_byte, &recv_flag, 0, 0);

		cout << "Server Sent [" << recv_byte << "bytes] : " << recv_buf << endl;
	}


	// Level 4:	Close
	// 소켓 닫기
	WSACleanup();
}