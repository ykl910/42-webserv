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

int Server::getClientMaxBodySize(const std::string& input) {
    return std::atoi(input.substr(0, input.length() - 1).c_str());
}

void Server::storeErrorPage(server& config) {
    std::string root(_attribute.location.root);

    _attribute.error_page.err_400 = root + "/" + config[ERROR][E_400];
    _attribute.error_page.err_403 = root + "/" + config[ERROR][E_403];
    _attribute.error_page.err_404 = root + "/" + config[ERROR][E_404];
    _attribute.error_page.err_500 = root + "/" + config[ERROR][E_500];
}

void Server::storeRedirection(server& config) {
    (void)config;
}

void Server::storeLocation(server& config) {
    _attribute.location.root = config[LOCATION][ROOT];
    _attribute.location.index = config[LOCATION][INDEX];
}

void Server::storeCgi(server& config) {
    (void)config;
}

void    Server::initSocket(void)
{
    _socket.createSocket(_attribute.host.c_str(), _attribute.port.c_str());
}

Server& Server::operator=(Server& other)
{
    if (this != &other) {
        t_serv_attr attr = other.getServerAttribute();
        _attribute.port = attr.port;
        _attribute.host = attr.host;
        _attribute.server_name = attr.server_name;
        _attribute.client_max_body_size = attr.client_max_body_size;
        _attribute.location.root = attr.location.root;
        _attribute.location.index = attr.location.index;
        _attribute.location = attr.location;
        _attribute.error_page = attr.error_page;
        _attribute.cgi = attr.cgi;
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
    storeLocation(config);
    storeErrorPage(config);
    storeRedirection(config);
    storeCgi(config);
}

Server::~Server() {}
