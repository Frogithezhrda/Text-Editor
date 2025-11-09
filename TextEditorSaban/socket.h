#pragma once
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")
#include "ui.h"


void initSocket();
void cleanSocket();