#pragma once

#include "WebServ.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include <sys/poll.h>
#include <sys/time.h>
#include <vector>

template <class Multiplexer>
class WebServ;

typedef std::vector<Server>::iterator serverIterator;

class Poll {
public:
    typedef std::vector<struct pollfd>::iterator pollIterator;

    void run();
    inline bool isSocketFd(int fd) const;
    std::vector<Server> getServer(void) const;

    void initServer(Config& config);
    void addClientToPoll(int clientFd, int serverFd);

    Poll(Config& config);
    ~Poll();

private:
    int                         _state;
    int                         _activity;
    std::vector<struct pollfd>  _pollFd;

    std::vector<Server>         _server;
    std::vector<int>            _listenFd;
    std::map<int, int>          _clientMap;
    std::map<int, Server>       _serverMap;
};
