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
    void addClientToPoll(int clientFd);

    Poll() {}
    Poll(std::vector<Server>& server);
    ~Poll();

private:
    int                         _activity;
    std::vector<struct pollfd>  _pollFd;
    std::vector<Server>         _server;
};
