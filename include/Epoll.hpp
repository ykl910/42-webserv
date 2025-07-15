#pragma once

#include "Socket.hpp"
#include "Error.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/epoll.h>
#include <vector>
#include <map>

#define MAXEVENTS 4096
#define BUFFERSIZE 512

template <class Multiplexer>
class WebServ;

class Epoll : public Socket {
public:
    typedef struct epoll_event epoll_ev;
    typedef std::vector<epoll_ev> vector;
    typedef std::map<int, std::string>::iterator buffersIt;
    typedef std::map<int, HttpRequest>::iterator requestsIt;

    void run(WebServ<Epoll>& server);

    int acceptClient();
    void addServerToEpoll();
    void createEpollInstance();

    void getRequest(int clientfd);
    void sendResponse(int &clientFd, HttpRequest &request);

    const int& getEpollFd(void) const;
    void addClientToEpoll(int const &clientFd);
    bool receivedCompleteRequest(std::string &rawData) const;
    void eventManager(epoll_ev &event);

    Epoll();
    ~Epoll();

private:
    int _epollFd;
    std::map<int, std::string> _buffers;
    std::map<int, HttpRequest> _requests;
    std::map<int, bool> _gotFullRequest;
    vector _eventsQueue;
};
