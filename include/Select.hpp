#pragma once

#include "HttpManager.hpp"
#include "WebServ.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "Signal.hpp"
#include <sys/select.h>
#include <algorithm>
#include <vector>

typedef std::vector<Server>::iterator serverIterator;

template <class Multiplexer>
class WebServ;

class Select {
public:
    typedef std::vector<int>::iterator selectIterator;

    std::vector<Server> getServer(void) const;

    void run();
    void initServer(Config& config);
    void addClientToSelect(int clientFd, int serverFd);
    void removeClientFromSelect(int client, size_t& i);
    inline bool isSocketFd(int fd) const;

    Select(Config& config);
    ~Select();

private:

    int                     _maxFd;
    int                     _activity;
    fd_set                  _readFds;
    fd_set                  _writeFds;
    fd_set                  _exceptFds;
    struct timeval          _tv;

    std::vector<int>        _listenFd;
    std::vector<int>        _newClientFd;
    std::vector<int>        _clientFd;

    std::vector<Server>     _server;
    std::map<int, int>      _clientState;
    std::map<int, bool>     _persistance;
    std::map<int, int>      _clientMap;
    std::map<int, Server>   _serverMap;
};

/*
  fd_set
       A structure type that can represent a set of file descriptors.
       According  to  POSIX, the  maximum  number  of  file  descriptors in an
       fd_set structure is the value of the macro FD_SETSIZE.
*/
