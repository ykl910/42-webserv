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

template <class Multiplexer>
class WebServ : public Multiplexer {
public:
    void    runMultiplexer(void) {
        this->run(*this);
    }

    HttpRequest receiveHttpRequest(int &clientFd);
    bool receivedCompleteRequest(std::string &rawData) const;
    void sendHttpResponse(int &clientFd, HttpRequest &request);

    void printServerStatus(const char* multiplexer) const;

    WebServ();
    WebServ(const char* configFile);
    ~WebServ();
};

#include "../src/WebServ.tpp"