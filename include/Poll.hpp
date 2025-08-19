#pragma once

#include "Socket.hpp"
#include "Error.hpp"
#include <sys/poll.h>
#include <sys/time.h>
#include <vector>

template <class Multiplexer>
class WebServ;

class Poll {
public:
    typedef std::vector<struct pollfd>::iterator pollIterator;

    void run();
    void addClientToPoll(int clientFd);

    Poll();
    ~Poll();

private:
    int                         _activity;
    std::vector<struct pollfd>  _pollFd;
};
