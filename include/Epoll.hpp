#pragma once

#include "Error.hpp"
#include <sys/epoll.h>

class WebServ;

class Epoll : public Error {
public:
    void run(WebServ& server);
    const int& getEpollFd(void) const;

    Epoll();
    ~Epoll();

private:
    int _epollFd;
    struct epoll_event server_ev;

};