#pragma once

#include "Error.hpp"
#include <sys/socket.h>
#include <unistd.h>

class Socket : public Error {
public:
    int  getServerFd() const;

    Socket();
    ~Socket();

private:
    bool _isBound;
    bool _isListening;

    int _serverFd;

    struct addrinfo _hints;
};
