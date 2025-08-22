#include "../include/Server.hpp"

int Server::getSocketFd(void) const {
    return _socket.getSocketFd();
}

static int getClientMaxBodySize(const std::string& input) {
    return std::atoi(input.substr(0, input.length() - 1).c_str());
}

static void storeErrorPage(void) {
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

Server::Server(server& config)
{
    _attribute.port = config[SERVER][LISTEN];
    _attribute.host = config[SERVER][HOST];
    _attribute.server_name = config[SERVER][SERVER_NAME];
    _attribute.client_max_body_size =
    getClientMaxBodySize(config[SERVER][CLIENT_MAX_BODY_SIZE]);
    storeErrorPage();
    storeRedirection();
    storeLocation();
    storeCgi();
}

Server::~Server() {
    std::cout << "Server destructor called\n";
}
