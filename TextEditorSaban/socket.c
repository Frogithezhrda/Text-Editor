#include "socket.h"

void initSocket()
{
	WSADATA wsa_data = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		showMessage("Failed to initialize Socket!");
	}
}

void cleanSocket()
{
	WSACleanup();
}
