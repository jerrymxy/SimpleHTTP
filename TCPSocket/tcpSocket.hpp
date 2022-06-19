#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

// Windows Socket DLL
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")
#include <WS2tcpip.h>
#include <iostream>

#define err(errmsg) std::cerr << errmsg << " failed, code " << WSAGetLastError() << std::endl

// Startup
bool initSocket();
// Close
bool cleanSocket();
// Create server socket
SOCKET createServerSocket();
// Create client socket
SOCKET createClientSocket(const char* ip_addr, const u_short port);

#endif // !_TCPSOCKET_H_
