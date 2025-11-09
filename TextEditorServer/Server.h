#pragma once
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")
#include <Windows.h>

struct Socket
{
	SOCKET sock;
	struct sockaddr_in sockAdd;
	bool isValid;
} Socket;


class Server
{
public:
	Server();
	~Server();

private:

};

Server::Server()
{
}

Server::~Server()
{
}