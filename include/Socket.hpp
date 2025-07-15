#pragma once

#include "Config.hpp"
#include "Error.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

class Socket : public Config {
public:
    int  getServerFd() const;
    void createAndBind();
    void setSocketOPt();
    void setOnListening();

    typedef struct addrinfo addrinfo;
    Socket();
    ~Socket();

private:
    int _serverFd;
    addrinfo _hints;
};
