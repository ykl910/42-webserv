#pragma once

#include "textFormatting.hpp"
#include "HttpRequest.hpp"
#include "Signal.hpp"
#include "Select.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "AError.hpp"
#include "Epoll.hpp"
#include "Poll.hpp"
#include "CGI.hpp"

#define PORT 8080

template <class Multiplexer>
class WebServ : public Signal, public Multiplexer {
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