#pragma once
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <cstring>
#include <vector>

#define PORT 8080

class ServerSocket {
    private:
        int serverFd;
        std::vector<int> clientFds;
        struct sockaddr_in serverAddress;
        int addrlen;
        bool isBound;
        bool isListening;

    public:
        ServerSocket();
        ~ServerSocket();
        int getServerFd() const;
        void bindAndListen();
        void acceptClient();
};
