#include "ServerSocket.hpp"

ServerSocket::ServerSocket() {
	if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(PORT);
	isBound = 0;
	isListening = 0;
	addrlen = sizeof(serverAddress);
}

ServerSocket::~ServerSocket() {
	if(serverFd)
		close(serverFd);
	if(clientFd)
		close(clientFd);
}

int ServerSocket::getServerFd() const {
	return serverFd;
}

void ServerSocket::bindAndListen(int port) {
	if(bind(serverFd, (struct sockaddr *)&serverAddress, addrlen) < 0)
	{
		perror("socket failed");
		close(serverFd);
		exit(EXIT_FAILURE);
	}
	if (listen(serverFd, 3) < 0) {
		perror("listen");
		close(serverFd);
		exit(EXIT_FAILURE);
	}
}

int ServerSocket::acceptClient() {
	clientFd = accept(serverFd, (struct sockaddr *)&serverAddress, (socklen_t *)addrlen);
	if (clientFd < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
}
