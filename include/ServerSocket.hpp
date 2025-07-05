#pragma once

#include "textFormatting.hpp"
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <vector>

#define PORT 8080

class ServerSocket {
public:
    void printError() const;
    void printServerStatus(const char* multiplexer) const;
    void executeCGI();
    void bindAndListen();
    int getServerFd() const;

    void acceptClientPoll();
    void acceptClientEpoll();
    void acceptClientSelect();

    void initSignalHandler();
    void initServer(const std::string& config_file);

    ServerSocket(const char* config_file);
    ~ServerSocket();

private:
    bool isBound;
    bool isListening;

    int addrlen;
    int serverFd;

    std::string config_file;
    std::vector<int> clientFds;

    struct sockaddr_in serverAddress;
    typedef std::vector<int>::iterator fdsIterator;
};

void    run_using_select(ServerSocket& server);
void    run_using_poll(ServerSocket& server);
void    run_using_epoll(ServerSocket& server);