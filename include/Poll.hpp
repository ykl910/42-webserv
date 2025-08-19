#pragma once

#include "WebServ.hpp"
#include "Socket.hpp"
#include "Error.hpp"
#include <sys/poll.h>
#include <sys/time.h>
#include <vector>

template <class Multiplexer>
class WebServ;

typedef struct s_server         t_server;
typedef std::vector<t_server>   config;
typedef config::iterator        configIterator;

class Poll {
public:
    typedef std::vector<struct pollfd>::iterator pollIterator;

    void run();
    void addClientToPoll(int clientFd);

    Poll() {}
    Poll(config& server);
    ~Poll();

private:
    int                         _activity;
    std::vector<struct pollfd>  _pollFd;
    config                      _server;
};
