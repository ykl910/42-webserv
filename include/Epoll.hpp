#pragma once

#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/epoll.h>
#include <vector>

#define MAXEVENTS 4096

template <class Multiplexer>
class WebServ;

class Epoll : public Socket {
public:
    void createEpollInstance();
    int acceptClient();
    void addServerToEpool();
    void addClientToEpool(int const &clientFd);
    void run(WebServ<Epoll>& server);
    HttpRequest receiveHttpRequest(int &clientFd);
    void sendHttpResponse(int &clientFd, HttpRequest &request);
    bool receivedCompleteRequest(std::string &rawData) const;
    const int& getEpollFd(void) const;

    typedef struct epoll_event epoll_ev;
    typedef std::vector<epoll_ev> vector;

    Epoll();
    ~Epoll();

private:
    int _epollFd;
};
