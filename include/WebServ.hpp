#pragma once

#include "textFormatting.hpp"
#include "Select.hpp"
#include "Error.hpp"
#include "utils.hpp"
#include "Epoll.hpp"
#include "Poll.hpp"
#include "CGI.hpp"

#define PORT 8080

template <class Multiplexer>
class WebServ {
public:
    void printServerStatus(const char* multiplexer, const char* configFilePath) const;

    WebServ(const char* configFilePath, const char* multiplexer);
    ~WebServ();

private:
    Config      _config;
    Multiplexer _multiplexer;
};
#include "../src/WebServ.tpp"
