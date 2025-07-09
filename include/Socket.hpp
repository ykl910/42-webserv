#pragma once

#include "Error.hpp"

class Socket : public Error {
public:
    Socket();
    ~Socket();

private:
    bool _isBound;
    bool _isListening;

    int _serverFd;

    struct addrinfo _hints;
    struct sockaddr_in _serverAddress;
};
