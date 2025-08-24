#pragma once

#include "Config.hpp"
#include "Error.hpp"
#include "Signal.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

class Socket {
public:
    int acceptClient();
    int getSocketFd() const;
    void createSocket(const char* host, const char* port);
    void setSocketOpt();

    Socket();
    ~Socket();

private:
    int              _socketFd;
    struct addrinfo  _hints;
};
