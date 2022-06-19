#include "tcpSocket.hpp"

// Settings for server
const int PORT = 12345;
const char* ADDR = "127.0.0.1";

bool initSocket() {
	// Windows Sockets Asynchronous
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		err("WSAStartup");
		return false;
	}

	return true;
}

bool cleanSocket() {
	if (WSACleanup() != 0) {
		err("WSACleanup");
		return false;
	}
	return true;
}

SOCKET createServerSocket() {
	// TCP Socket, 3rd param: IPPROTO_TCP or 0 to auto decide
	SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

	// UDP Socket, 3rd param: IPPROTO_UDP or 0 to auto decide
	// SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (tcp_socket == INVALID_SOCKET) {
		err("socket");
		return tcp_socket;
	}

	// Save IPv4 address & port
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	// serv_addr.sin_addr.S_un.S_addr = inet_addr(ADDR);
	// For Visual Studio 2013+
	inet_pton(AF_INET, ADDR, &serv_addr.sin_addr.S_un.S_addr);

	// Bind socket with IP addr & port
	if (bind(tcp_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
		err("bind");
		return 0;
	}
	const int BACKLOG = 20; // Max request queue length
	listen(tcp_socket, BACKLOG);

	return tcp_socket;
}

SOCKET createClientSocket(const char* ip_addr, const u_short port) {
	// TCP Socket, 3rd param: IPPROTO_TCP or 0 to auto decide
	SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

	// UDP Socket, 3rd param: IPPROTO_UDP or 0 to auto decide
	// SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (tcp_socket == INVALID_SOCKET) {
		err("socket");
		return tcp_socket;
	}

	// Save IPv4 address & port
	struct sockaddr_in cli_addr;
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons(port);
	// cli_addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);
	inet_pton(AF_INET, ip_addr, &cli_addr.sin_addr.S_un.S_addr);

	if (connect(tcp_socket, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) == SOCKET_ERROR) {
		err("connect");
		return 0;
	}

	return tcp_socket;
}
