#pragma once
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <cstring>
#include <vector>

class socket {
	private:
		int sockFd;
		sockaddr_in serverAddress;
		int isBound;
		int isListen;
	public:
		socket();
		~socket();
		int getSockFd() const { return sockFd;};
		


};