#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <vector>

#define PORT 8080

class ServerSocket {
	private:
		int serverFd;
		int clientFd;
		struct sockaddr_in serverAddress;
		int addrlen;
		bool isBound;
		bool isListening;

	public:
		ServerSocket();
		~ServerSocket();
		int getServerFd() const { return serverFd;};
		void bindAndListen(int port);
		int acceptClient();
};
