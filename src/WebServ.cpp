#include "../include/WebServ.hpp"

void WebServ::printServerStatus(const char* multiplexer) const {
    std::cout << BOLD WHITE << "Server status: "
    << BOLD ITALIC GREEN << "running\n" << DEFAULT
    << BOLD WHITE << "Port: "
    << BOLD ITALIC BLUE  << "8080\n" << DEFAULT
    << BOLD WHITE << "Multiplexer: "
    << BOLD ITALIC BLUE  << multiplexer << DEFAULT
    << std::endl;
}

void    WebServ::runEpoll() {
    this->_epoll.run(*this);
}

void    WebServ::runPoll() {
    this->_poll.run(*this);
}

void    WebServ::runSelect() {
    this->_select.run(*this);
}

WebServ::WebServ(const char* configFile)
    : _config(configFile) {
}

WebServ::~WebServ() {
}
