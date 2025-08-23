#pragma once

#include "textFormatting.hpp"
#include "Select.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Error.hpp"
#include "utils.hpp"
#include "Epoll.hpp"
#include "Poll.hpp"
#include "CGI.hpp"

#define POST42dotNET "./www/post42.net/"
#define WEBSERVdotNET "./www/webserv.net/"
// #define POST42dotNET WEBSERVdotNET

typedef std::vector<Server*>::iterator serverIterator;

template <class Multiplexer>
class WebServ {
public:
    void printServerStatus(const char* multiplexer, const char* configFilePath);

    WebServ(Config& config, const char* multiplexer);
    ~WebServ();

private:
    Multiplexer _multiplexer;
};
#include "../src/WebServ.tpp"
