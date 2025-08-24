#pragma once

#include "HttpManager.hpp"
#include "WebServ.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "Signal.hpp"
#include <sys/select.h>
#include <vector>

typedef std::vector<Server>::iterator serverIterator;

template <class Multiplexer>
class WebServ;

class Select {
public:
    void run();
    std::vector<Server> getServer(void) const;
    void createServer(Config& config);
    bool isSocketFd(int fd) const;

    Select(Config& config);
    ~Select();

private:
    typedef std::vector<int>::iterator selectIterator;

    int                     _maxFd;
    int                     _activity;
    fd_set                  _readFds;
    struct timeval          _tv;
    std::vector<int>        _listenFd;
    std::vector<int>        _clientFd;
    std::vector<Server>    _server;
    std::map<int, Server*>   _clientToServer;
};

/*
  fd_set
       A structure type that can represent a set of file descriptors.
       According  to  POSIX, the  maximum  number  of  file  descriptors in an
       fd_set structure is the value of the macro FD_SETSIZE.
*/
