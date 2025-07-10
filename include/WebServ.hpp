#pragma once

#include "textFormatting.hpp"
#include "HttpRequest.hpp"
#include "Signal.hpp"
#include "Select.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Epoll.hpp"
#include "Error.hpp"
#include "Poll.hpp"
#include "CGI.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <netdb.h>
#include <cstdio>

#define PORT 8080
#define BUFFERSIZE 4096

class WebServ {
public:
    void runPoll();
    void runEpoll();
    void runSelect();

    void acceptClient();
    HttpRequest receiveHttpRequest(int &clientFd);
    bool receivedCompleteRequest(std::string &rawData) const;
    void sendHttpResponse(int &clientFd, HttpRequest &request);

    void printServerStatus(const char* multiplexer) const;

    WebServ(const char* configFile);
    ~WebServ();

private:
    CGI     _cgi;
    Poll    _poll;
    Epoll   _epoll;
    Error   _error;
    Select  _select;
    Signal  _signal;
    Config  _config;
    Socket  _socket;
};
