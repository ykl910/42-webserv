#pragma once

#include "Config.hpp"
#include "AError.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

class Socket : public Config {
public:
    Socket();
    ~Socket();

    int  getServerFd() const;
    void createAndBind();
    void setSocketOPt();
    void setOnListening();

    typedef struct addrinfo addrinfo;

private:
    bool _isBound;
    bool _isListening;

    int _serverFd;

    addrinfo _hints;
};
