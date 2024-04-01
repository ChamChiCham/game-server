#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <unordered_map>
#pragma comment (lib, "WS2_32.LIB")

constexpr short PORT = 4000;
constexpr int BUFSIZE = 256;

bool b_shutdown = false;

class SESSION;

// { client Overlapped pointer : ID }
std::unordered_map<LPWSAOVERLAPPED, int> g_session_map;

// { ID : session }
std::unordered_map<int, SESSION> g_players;

void CALLBACK send_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK recv_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void print_error(const char* msg, int err_no);


struct Queen {
	std::pair<unsigned char, unsigned char> pos{};
	unsigned char status{};
};

// { ID : Data }
std::unordered_map<int, Queen> g_queens;



/**
*  client에게 send하기 위해 overlapped/wsabuf/buf을 묶은 자료형.
*/
class EXP_OVER
{
public:
	WSAOVERLAPPED over;
	WSABUF wsabuf[1];
	char buf[BUFSIZE];
	EXP_OVER(int s_id, char* mess, int m_size)
	{
		ZeroMemory(&over, sizeof(over));
		wsabuf[0].buf = buf;
		wsabuf[0].len = m_size + 2;

		buf[0] = m_size + 2;
		buf[1] = s_id;

		memcpy(buf + 2, mess, m_size);
	}
};

/**
* server에서 client에게 recv와 send를 해주는 session.
*/
class SESSION {
	char buf[BUFSIZE];
	WSABUF wsabuf[1];
	SOCKET client_s;
	WSAOVERLAPPED over;
	int send_size;

public:
	SESSION(SOCKET s, int my_id) : client_s(s) {
		g_session_map[&over] = my_id;
		wsabuf[0].buf = buf;
		wsabuf[0].len = BUFSIZE;
	}
	SESSION() {
		std::cout << "ERROR";
		exit(-1);
	}
	~SESSION() { closesocket(client_s); }

	// recv
	void do_recv()
	{
		DWORD recv_flag = 0;
		ZeroMemory(&over, sizeof(over));
		int res = WSARecv(client_s, wsabuf, 1, nullptr, &recv_flag, &over, recv_callback);
		if (0 != res) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				print_error("WSARecv", WSAGetLastError());
		}
	}

	// send
	void do_send(int s_id, char* mess, int recv_size)
	{
		auto b = new EXP_OVER(s_id, mess, recv_size);
		int res = WSASend(client_s, b->wsabuf, 1, nullptr, 0, &b->over, send_callback);
		if (0 != res) {
			print_error("WSASend", WSAGetLastError());
		}
	}

	void do_send(int s_id)
	{
		do_send(s_id, buf, send_size);
	}

	// --
	// recv/send process function
	// --

	void proccessMessage()
	{
		int id = g_session_map[&over];
		Queen queen = g_queens[id];

		if (buf[0] == 'U') {
			if (buf[1] == 'U') {
				queen.status &= ~0x01;
			}
			else if (buf[1] == 'D') {
				queen.status &= ~0x02;
			}
			else if (buf[1] == 'L') {
				queen.status &= ~0x04;
			}
			else if (buf[1] == 'R') {
				queen.status &= ~0x08;
			}
		}
		else if (buf[0] == 'D') {
			if (buf[1] == 'U' && !(queen.status & 0x01)) {
				queen.status |= 0x01;
				if (queen.pos.second > 0) {
					queen.pos.second--;
				}
			}
			else if (buf[1] == 'D' && !(queen.status & 0x02)) {
				queen.status |= 0x02;
				if (queen.pos.second < 7) {
					queen.pos.second++;
				}
			}
			else if (buf[1] == 'L' && !(queen.status & 0x04)) {
				queen.status |= 0x01;
				if (queen.pos.first > 0) {
					queen.pos.first--;
				}
			}
			else if (buf[1] == 'R' && !(queen.status & 0x08)) {
				queen.status |= 0x08;
				if (queen.pos.first < 7) {
					queen.pos.first++;
				}
			}
		}
		
	}

	void setSendBuffer(int data_id)
	{
		Queen queen = g_queens[data_id];

		buf[0] = 'M';
		buf[1] = queen.pos.first;
		buf[2] = queen.pos.second;
		send_size = 3;
	}

	// send message to all client
	void broadcast(int data_id)
	{
		for (auto& p : g_players) {
			p.second.setSendBuffer(data_id);
			p.second.do_send(g_session_map[&over]);
		}
	}
};

// 에러가 났을때 멈줘주는 함수.
void print_error(const char* msg, int err_no)
{
	WCHAR* msg_buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L" : 에러 : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}

// send 완료 후 실행되는 callback
void CALLBACK send_callback(DWORD err, DWORD sent_size,
	LPWSAOVERLAPPED pover, DWORD recv_flag)
{
	if (0 != err) {
		print_error("WSASend", WSAGetLastError());
	}
	auto b = reinterpret_cast<EXP_OVER*>(pover);
	delete b;
}

// recv 완료 후 실행되는 callback
void CALLBACK recv_callback(DWORD err, DWORD recv_size,
	LPWSAOVERLAPPED pover, DWORD recv_flag)
{
	if (0 != err) {
		print_error("WSARecv", WSAGetLastError());
	}
	int my_id = g_session_map[pover];
	Queen& queen = g_queens[my_id];

	if (0 == recv_size) {
		g_players.erase(my_id);
		return;
	}
	
	// --
	// recv process
	// --

	// TODO: 정보 처리
	auto pos = queen.pos;

	g_players[my_id].proccessMessage();

	if (pos != queen.pos) {
		g_players[my_id].broadcast(my_id);
	}

	g_players[my_id].do_recv();
}

int main()
{
	std::wcout.imbue(std::locale("korean"));

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	// create socker
	SOCKET server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

	// bind
	SOCKADDR_IN server_a;
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);
	server_a.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));

	// listen
	listen(server_s, SOMAXCONN);


	int addr_size = sizeof(server_a);
	int id = 0;

	// accept client socket
	while (false == b_shutdown) {
		SOCKET client_s = WSAAccept(server_s, reinterpret_cast<sockaddr*>(&server_a), &addr_size, nullptr, 0);
		g_players.try_emplace(id, client_s, id);
		g_queens[id] = Queen{};

		g_players[id].broadcast(id);
		g_players[id++].do_recv();
	}
	g_players.clear();
	closesocket(server_s);
	WSACleanup();
}