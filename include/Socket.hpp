#pragma once

#include "Config.hpp"
#include "Error.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

class Socket {
public:
    int getSocketFd() const;
    int acceptClient();
    void createAndBind();
    void setSocketOpt();

    Socket();
    ~Socket();

private:
    int             _socketFd;
    struct addrinfo _hints;
};
