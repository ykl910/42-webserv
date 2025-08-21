#pragma once

#include "Config.hpp"
#include "Error.hpp"
#include "Signal.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

class Socket {
public:
    int getSocketFd() const;
    int acceptClient();
    void createSocket(const char* host, const char* port);
    void setSocketOpt();

    Socket(const char* host, const char* port);
    ~Socket();

private:
    int              _socketFd;
    struct addrinfo  _hints;
};
