#include "../../include/Server.hpp"

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

void Server::storeErrorPage(server& config, size_t locationNbr) {
    std::string root(_attribute.location[0].root);

    _attribute.error_page.err_400 = root + "/" + config[ERROR + locationNbr][E_400][VALUE];
    _attribute.error_page.err_403 = root + "/" + config[ERROR + locationNbr][E_403][VALUE];
    _attribute.error_page.err_404 = root + "/" + config[ERROR + locationNbr][E_404][VALUE];
    _attribute.error_page.err_500 = root + "/" + config[ERROR + locationNbr][E_500][VALUE];
}

void Server::storeRedirection(server& config) {
    (void)config;
}

#define GET 0
#define POST 1
#define DELETE 2

bool Server::methodAlreadyDefined(uint8_t mask, size_t method)
{
    return mask &= 1 << method;
}

void Server::storeLocation(server& config, size_t locationNbr)
{
    for (size_t i = 0; i < locationNbr; ++i) {
        t_location newLocation;

        newLocation.path = config[LOCATION + i][PATH][0];
        newLocation.root = config[LOCATION + i][ROOT][0];
        newLocation.index = config[LOCATION + i][INDEX][0];

        if (config[LOCATION + i][AUTOINDEX][0] == "on")
            newLocation.autoindex = true;
        else if (config[LOCATION + i][AUTOINDEX][0] == "off")
            newLocation.autoindex = false;

        std::stringstream methodDirective(config[LOCATION + i][METHOD][0]);
        std::string method;
        newLocation.method = 0;
        while (methodDirective >> method) {
            if (method == "GET" && !methodAlreadyDefined(newLocation.method, GET))
                newLocation.method |= 1 << GET;
            else if (method == "POST" && !methodAlreadyDefined(newLocation.method, POST))
                newLocation.method |= 1 << POST;
            else if (method == "DELETE" && !methodAlreadyDefined(newLocation.method, DELETE))
                newLocation.method |= 1 << DELETE;
        }
        _attribute.location.push_back(newLocation);
    }
}

void Server::storeCgi(server& config, size_t cgiNbr) {
    (void)config;
    (void)cgiNbr;
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
        _attribute.location[0].root = attr.location[0].root;
        _attribute.location[0].index = attr.location[0].index;
        _attribute.location = attr.location;
        _attribute.error_page = attr.error_page;
        _attribute.cgi = attr.cgi;
    }
    return *this;
}

Server::Server(server& config, size_t locationNbr, size_t cgiNbr) : _socket()
{
    _attribute.port = config[SERVER][LISTEN][0];
    _attribute.host = config[SERVER][HOST][0];
    _attribute.server_name = config[SERVER][SERVER_NAME][0];
    _attribute.client_max_body_size =
    getClientMaxBodySize(config[SERVER][CLIENT_MAX_BODY_SIZE][0]);
    storeLocation(config, locationNbr);
    storeErrorPage(config, locationNbr);
    storeRedirection(config);
    storeCgi(config, cgiNbr);
}

Server::~Server() {}
