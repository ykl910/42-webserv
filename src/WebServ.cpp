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

int WebServ::getServerFd() const {
    return this->_serverFd;
}

void    WebServ::parseConfigFile(const std::string& configFile) {
    (void)configFile;
    //TODO: Parse the conf_file and extract the port we will use (ex: 8080)
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
    : _configFile(configFile), _portServie("8080"){

    parseConfigFile(this->_configFile);

    bzero(&this->_hints, sizeof(this->_hints));
    this->_hints.ai_family = AF_INET;
    this->_hints.ai_socktype = SOCK_STREAM;
    this->_hints.ai_flags = AI_PASSIVE;
}

WebServ::~WebServ() {
    if (this->_serverFd)
        close(this->_serverFd);
    if(this->_epollFd)
        close(this->_epollFd);
    for (fdsIterator it = this->_clientFds.begin(); it != this->_clientFds.end(); ++it)
        close(*it);
}
