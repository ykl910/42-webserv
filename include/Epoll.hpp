#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
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

    std::vector<Server> getServer(void) const;
    void run();
    bool isSocketFd(int fd) const;
    void addClientToEpoll(int const &clientFd, Server serv);

    void initServer(Config& config);
    void eventManager(epoll_ev &event);

    Epoll(Config& config);
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
    std::vector<int>            _listenFd;
    std::vector<Server>         _server;
    std::map<int, Server>       _clientToServer;
};


// #pragma once

// #include "Socket.hpp"
// #include "Error.hpp"
// #include "utils.hpp"
// #include "HttpRequest.hpp"
// #include "HttpResponse.hpp"
// #include <sys/epoll.h>
// #include <vector>

// #define MAXEVENTS 4096
// #define BUFFERSIZE 512

// template <class Multiplexer>
// class WebServ;

// class Epoll : public Socket {
// public:
//     typedef struct epoll_event epoll_ev;
//     typedef std::vector<epoll_ev> vector;
//     typedef std::map<int, std::string>::iterator buffersIt;
//     typedef std::map<int, HttpRequest>::iterator requestsIt;

//     void run(WebServ<Epoll>& server);
//     void enableWriteEvent(int clientFd);
//     void disableWriteEvent(int clientFd);
//     void createEpollInstance();
//     void addServerToEpoll();
//     int acceptClient();
//     void eventManager(epoll_ev &event);
//     void getRequest(int clientfd);
//     void sendResponse(int clientFd, HttpRequest request);

//     const int& getEpollFd(void) const;
//     void addClientToEpoll(int const &clientFd);
//     bool receivedCompleteRequest(std::string &rawData) const;

//     Epoll(const char *configFilePath);
//     ~Epoll();

// private:
//     int _epollFd;
//     vector _eventsQueue;
//     std::map<int, std::string> _buffers;
//     std::map<int, HttpRequest> _requests;
//     std::map<int, bool> _gotFullRequest;
//     std::map<int, bool> _gotResponse;
//     std::map<int, HttpResponse> _responses;
//     std::map<int, int> _pendingResponse;
// };
