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
#include "../include/HttpRequest.hpp"

#define PORT 8080
#define BUFFERSIZE 4096

class WebServ {
public:
    void printServerStatus(const char* multiplexer) const;
    void executeCGI();
    int getServerFd() const;

    void multiplexPoll();
    void multiplexEpoll();
    void multiplexSelect();

    void initSignalHandler();
    void initServer();
    void parseConfigFile(const std::string& configFile);

    WebServ(const char* configFile);
    ~WebServ();

private:

    bool _isBound;
    bool _isListening;

    int _addrlen;
    int _serverFd;

    std::string _configFile;
    std::vector<int> _clientFds;

    std::string _portServie;
    struct addrinfo _hints;
    struct sockaddr_in _serverAddress;

    typedef std::vector<int>::iterator fdsIterator;

    void printError() const;
    void printErrorAndThrow(std::string const &context) const;
    void printGaiErrorAndThrow(std::string const &context, int &status) const;

    bool receivedCompleteRequest(std::string &rawData) const;
    void acceptClient();
    HttpRequest receiveHttpRequest(int &clientFd);
    void sendHttpResponse(int &clientFd, HttpRequest &request);
};

void    initSignalHandler(void);

void    run_using_select(WebServ& server);
void    run_using_poll(WebServ& server);
void    run_using_epoll(WebServ& server);
