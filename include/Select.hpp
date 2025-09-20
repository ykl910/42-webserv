#pragma once

#include "HttpManager.hpp"
#include "WebServ.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "Signal.hpp"
#include "utils.hpp"

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
    void findSocketPort(Socket& socketReference,
                               std::vector<Server>& servers, std::string port);
    void addClientToSelect(int clientFd);
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

    struct timeval          _timeout;

    std::vector<int>        _listenFd;
    std::vector<int>        _clientFd;
    std::vector<int>        _persistantClientFd;

    std::map<int, bool>     _persistance;
    std::map<int, int>      _clientState;

    std::vector<Server>     _server;
    std::map<int, int>      _clientMap;
    std::map<int, Server>   _serverMap;
};
