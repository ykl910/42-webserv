#pragma once

#include "textFormatting.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Signal.hpp"
#include "Select.hpp"
#include "Error.hpp"
#include "Epoll.hpp"
#include "Poll.hpp"
#include "CGI.hpp"
#include "utils.hpp"

#define PORT 8080

template <class Multiplexer>
class WebServ {
private:
    Signal _signalHandler;
    Multiplexer _multiplexer;

public:
    WebServ(const char* configFilePath);
    ~WebServ();
    void printServerStatus(const char* multiplexer, const char* configFilePath) const;
    void runMultiplexer() {
        _multiplexer.run(*this);
    }
};
#include "../src/WebServ.tpp"
