#pragma once

#include "textFormatting.hpp"
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include <netdb.h>
#include <cstdio>
#include <cerrno>
#include <vector>

#define PORT 8080

class WebServ {
public:
    void printError() const;
    void printErrorAndThrow(std::string const &context) const;
    void printGaiErrorAndThrow(std::string const &context) const;
    void printServerStatus(const char* multiplexer) const;

    void executeCGI();
    void bindAndListen();
    int getServerFd() const;

    void acceptClientPoll();
    void acceptClientEpoll();
    void acceptClientSelect();

    void initSignalHandler();
    void initServer(const std::string& config_file);

    WebServ(const char* config_file);
    ~WebServ();

private:
    bool _isBound;
    bool _isListening;

    int _addrlen;
    int _serverFd;

    std::string _config_file;
    std::vector<int> _clientFds;

    struct addrinfo _hints;
    struct addrinfo *_servInfos;
    struct sockaddr_in _serverAddress;

    typedef std::vector<int>::iterator fdsIterator;

    bool receivedCompleteRequest(std::string &rawData) const;
    void receiveHttpRequest();
    void sendHttpResponse();

};

void    run_using_select(WebServ& server);
void    run_using_poll(WebServ& server);
void    run_using_epoll(WebServ& server);
