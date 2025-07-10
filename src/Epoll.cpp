#include "../include/Epoll.hpp"
#include "../include/WebServ.hpp"

const int&    Epoll::getEpollFd(void) const {
    return this->_epollFd;
}

void Epoll::acceptClient() {

    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize;

    clientAddrSize = sizeof(clientAddr);

    int clientFd = accept(this->getServerFd(), reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrSize);
    if (clientFd == -1)
        this->printErrorAndThrow("accept");

    struct epoll_event client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = clientFd;

    if (epoll_ctl(this->getEpollFd(), EPOLL_CTL_ADD, clientFd, &client_ev) == -1)
        this->printErrorAndThrow("epoll_ctl");

}


void    Epoll::run(WebServ& server) {
    server.printServerStatus("epoll");
    std::vector<struct epoll_event> events(4096); //!This server is not for pussies

    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1)
        this->printErrorAndThrow("epoll_create1");

    // addServerToEpoll
    server_ev.events = EPOLLIN;
    server_ev.data.fd = this->getServerFd();

    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->getServerFd(), &server_ev) == -1)
        this->printErrorAndThrow("epoll_ctl");

    while(true) {

        int nbEvents = epoll_wait(this->_epollFd, events.data(), events.size(), -1);
        if(nbEvents == -1)
            this->printErrorAndThrow("epoll_wait");
        for(int i = 0; i < nbEvents; ++i){

            int fd = events[i].data.fd;

            if(fd == this->getServerFd())
                this->acceptClient();
           // else
                //Do some shit
        }

    }

    // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    close(this->_epollFd);
}

Epoll::Epoll() {
    // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
}

Epoll::~Epoll() {
    if (this->_epollFd)
        close(this->_epollFd);

}