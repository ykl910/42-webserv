#pragma once

#include "Socket.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include "utils.hpp"
#include "Signal.hpp"
#include <sys/epoll.h>
#include <vector>

#define MAXEVENTS 4096
#define BUFFERSIZE 512

template <class Multiplexer>
class WebServ;

typedef std::vector<Server>::iterator serverIterator;

class Epoll {
public:
    typedef struct epoll_event epoll_ev;
    typedef std::vector<epoll_ev> vector;
    typedef std::map<int, std::string>::iterator buffersIt;
    typedef std::map<int, HttpRequest>::iterator requestsIt;

    void run();
    void addClientToEpoll(int const &clientFd);

    void enableWriteEvent(int clientFd);
    void disableWriteEvent(int clientFd);
    void eventManager(epoll_ev &event);
    void getRequest(int clientfd);
    void sendResponse(int clientFd, HttpRequest request);
    bool receivedCompleteRequest(std::string &rawData) const;

    Epoll() {}
    Epoll(std::vector<Server>& server);
    ~Epoll();

private:
    int                         _epollFd;
    int                         _nbEvents;
    vector                      _eventsQueue;
    std::map<int, std::string>  _buffers;
    std::map<int, bool>         _gotResponse;
    std::map<int, bool>         _gotFullRequest;
    std::map<int, int>          _pendingResponse;
    std::map<int, HttpRequest>  _requests;
    std::map<int, HttpResponse> _responses;
    std::vector<Server>         _server;
};
