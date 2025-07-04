#pragma once

#include "textFormatting.hpp"
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <vector>

#define PORT 8080

class ServerSocket {
public:
    void acceptClient();
    void bindAndListen();
    int getServerFd() const;

    ServerSocket();
    ServerSocket(const char *config_file);
    ServerSocket(const char *config_file, const char *multiplexer);
    ~ServerSocket();

private:
    int serverFd;
    std::vector<int> clientFds;
    struct sockaddr_in serverAddress;
    int addrlen;
    bool isBound;
    bool isListening;

    typedef std::vector<int>::iterator fdsIterator;

    void printError();
};
