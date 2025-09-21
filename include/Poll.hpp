#pragma once

#include "WebServ.hpp"
#include "signal.hpp"
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
    void enableWriteEvent(int clientFd);
    void disableWriteEvent(int clientFd);
    void findSocketPort(Socket& socketReference,
                            std::vector<Server>& servers, std::string port);
    void removeClientFromPoll(pollIterator& it);
    void addClientToPoll(int clientFd);

    Poll(Config& config);
    ~Poll();

private:
    int                             _activity;
    std::vector<struct pollfd>      _pollFd;
    std::map<int, struct pollfd>    _pollFdMap;
    std::vector<struct pollfd>      _newClientFd;

    std::map<int, bool>             _persistance;
    std::map<int, int>              _clientState;

    std::vector<Server>             _server;
    std::map<int, int>              _clientMap;
    std::map<int, Server>           _serverMap;
};
