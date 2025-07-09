#pragma once

#include <sys/epoll.h>

class WebServ;

class Epoll {
public:
    void run(WebServ& server);

    Epoll();
    ~Epoll();

private:

};