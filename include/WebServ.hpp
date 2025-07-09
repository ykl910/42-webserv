#pragma once

#include "textFormatting.hpp"
#include "HttpRequest.hpp"
#include "Signal.hpp"
#include "Select.hpp"
#include "Config.hpp"
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
#include <cstring>
#include <netdb.h>
#include <cstdio>
#include <cerrno>
#include <vector>

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

    void initServer();
    int  getServerFd() const;
    void parseConfigFile(const std::string& configFile);
    void printServerStatus(const char* multiplexer) const;

    WebServ(const char* configFile);
    ~WebServ();

private:
    bool _isBound;
    bool _isListening;

    // int _addrlen;
    int _serverFd;

    std::string _configFile;

    std::string _portServie;
    struct addrinfo _hints;
    struct sockaddr_in _serverAddress;

    // typedef std::vector<int>::iterator fdsIterator;

    CGI     _cgi;
    Poll    _poll;
    Epoll   _epoll;
    Error   _error;
    Select  _select;
    Signal  _signal;
    Config  _config;
};
