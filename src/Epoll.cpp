#include "../include/Epoll.hpp"
#include "../include/WebServ.hpp"

void    Epoll::run(WebServ& server) {
    server.printServerStatus("epoll");
}

Epoll::Epoll() {
    // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
}

Epoll::~Epoll() {

}