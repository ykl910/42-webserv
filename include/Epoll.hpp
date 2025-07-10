#pragma once

#include "Socket.hpp"
#include <sys/epoll.h>
#include <vector>

#define MAXEVENTS 4096

class WebServ;

class Epoll : public Socket {
public:
    void createEpollInstance();
    int acceptClient();
    void addServerToEpool();
    void addClientToEpool(int const &clientFd);
    void run(WebServ& server);
    const int& getEpollFd(void) const;

    typedef struct epoll_event epoll_ev;
    typedef std::vector<epoll_ev> vector;

    Epoll();
    ~Epoll();

private:
    int _epollFd;
};
