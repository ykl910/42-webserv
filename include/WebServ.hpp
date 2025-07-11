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
class WebServ {
private:
    Signal _signalHandler;
    Multiplexer _multiplexer;
    
public:
    WebServ();
    WebServ(const char* configFile);
    ~WebServ();
    void printServerStatus(const char* multiplexer) const;
    void runMultiplexer() {
        _multiplexer.run(*this);
    }
};
#include "../src/WebServ.tpp"
