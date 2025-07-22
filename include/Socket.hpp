#pragma once

#include "Config.hpp"
#include "Error.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

class Socket : public Config {
public:
    int  getSocketFd() const;
    void createAndBind();
    void setSocketOpt();
    void setOnListening();

    Socket(const char *configFilePath);
    ~Socket();

private:
    int             _socketFd;
    struct addrinfo _hints;
    // Config          _config;
};
