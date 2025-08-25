#include "../include/Server.hpp"

Socket& Server::getSocket(void) {
    return _socket;
}

int Server::getSocketFd(void) const {
    return _socket.getSocketFd();
}

t_serv_attr& Server::getServerAttribute(void) {
    return _attribute;
}

static int getClientMaxBodySize(const std::string& input) {
    return std::atoi(input.substr(0, input.length() - 1).c_str());
}

static void storeErrorPage(Server &serv, server &config) {
    serv.getServerAttribute().error_page.err_400 = config[ERROR][E_400];
    serv.getServerAttribute().error_page.err_403 = config[ERROR][E_403];
    serv.getServerAttribute().error_page.err_404 = config[ERROR][E_404];
    serv.getServerAttribute().error_page.err_500 = config[ERROR][E_500];
    return;
}

static void storeRedirection(void) {
    return;
}

static void storeLocation(void) {
    return;
}

static void storeCgi(void) {
    return;
}

void    Server::initSocket(void)
{
    _socket.createSocket(_attribute.host.c_str(), _attribute.port.c_str());
}

Server& Server::operator=(Server& other)
{
    if (this != &other) {
        t_serv_attr attr = other.getServerAttribute();
        this->_attribute.client_max_body_size = attr.client_max_body_size;
        this->_attribute.port = attr.port;
        this->_attribute.host = attr.host;
        this->_attribute.listen = attr.listen;
        this->_attribute.server_name = attr.server_name;
        this->_attribute.error_page = attr.error_page;
        this->_attribute.redirection = attr.redirection;
        this->_attribute.location = attr.location;
        this->_attribute.cgi = attr.cgi;
    }
    return *this;
}

Server::Server(server& config) : _socket()
{
    _attribute.port = config[SERVER][LISTEN];
    _attribute.host = config[SERVER][HOST];
    _attribute.server_name = config[SERVER][SERVER_NAME];
    _attribute.client_max_body_size =
    getClientMaxBodySize(config[SERVER][CLIENT_MAX_BODY_SIZE]);
    storeErrorPage(*this, config);
    storeRedirection();
    storeLocation();
    storeCgi();
}

Server::~Server() {}
