#pragma once

#include "Config.hpp"
#include "Error.hpp"
#include "Signal.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define ALREADY_USED 1

class Socket {
public:
    static std::vector<std::string> _portsUsed;

    int acceptClient();
    int getSocketFd() const;
    void createSocket(const char* host, const char* port, bool state);
    bool portAlreadyUsed(const std::string& port);
    void setSocketOpt();

    Socket();
    ~Socket();

private:
    int                             _socketFd;
    struct addrinfo                 _hints;
};
