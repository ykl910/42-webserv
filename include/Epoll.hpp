#pragma once

#include "HttpManager.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include "utils.hpp"
#include "Signal.hpp"

#include <sys/epoll.h>
#include <vector>

#define MAXEVENTS 4096
#define GOT_FULL_REQUEST 1

template <class Multiplexer>
class WebServ;

typedef std::vector<Server>::iterator serverIterator;

class Epoll {
public:
    typedef struct epoll_event epoll_ev;
    typedef std::vector<epoll_ev> vector;
    typedef std::map<int, std::string>::iterator buffersIt;
    typedef std::map<int, HttpRequest>::iterator requestsIt;

    std::vector<Server> getServer(void) const;
    void run();
    void enableWriteEvent(int clientFd);
    void disableWriteEvent(int clientFd);
    inline bool isSocketFd(int fd) const;
    void addClientToEpoll(int clientFd, int serverFd);
    void removeClientFromEpoll(int clientFd);
    void printFdError(int clientFd);
    void findSocketPort(Socket& socketReference, std::vector<Server>& servers, std::string port);

    void initServer(Config& config);
    void eventManager(epoll_ev &event);

    Epoll(Config& config);
    ~Epoll();

private:
    int                         _epollFd;
    int                         _nbEvents;
    vector                      _eventsQueue;

    std::map<int, bool>         _persistance;
    std::map<int, int>          _clientState;

    std::vector<Server>         _server;
    std::vector<int>            _listenFd;
    std::map<int, int>          _clientMap;
    std::map<int, Server>       _serverMap;
};
