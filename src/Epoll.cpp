#include "../include/Epoll.hpp"
#include "../include/WebServ.hpp"

const int&    Epoll::getEpollFd(void) const {
    return this->_epollFd;
}

int Epoll::acceptClient() {

    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize;

    clientAddrSize = sizeof(clientAddr);

    int clientFd = accept(this->getServerFd(), reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrSize);
    if (clientFd == -1)
        this->printErrorAndThrow("accept");

    return clientFd;
}

void Epoll::createEpollInstance(){

    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1)
        this->printErrorAndThrow("epoll_create1");
}

void Epoll::addServerToEpool(){

    epoll_ev server_ev;
    server_ev.events = EPOLLIN;
    server_ev.data.fd = this->getServerFd();

    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->getServerFd(), &server_ev) == -1)
        this->printErrorAndThrow("epoll_ctl");
}

void Epoll::addClientToEpool(int const &clientFd){

    epoll_ev client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = clientFd;

    if (epoll_ctl(this->getEpollFd(), EPOLL_CTL_ADD, clientFd, &client_ev) == -1)
        this->printErrorAndThrow("epoll_ctl");
}

void    Epoll::run(WebServ& server) {

    server.printServerStatus("epoll");

    this->createEpollInstance();
    this->addServerToEpool();

    vector eventsQueue(MAXEVENTS);

    while(true) {

        int nbEvents = epoll_wait(this->_epollFd, eventsQueue.data(), eventsQueue.size(), -1);
        if(nbEvents == -1)
            this->printErrorAndThrow("epoll_wait");
        for(int i = 0; i < nbEvents; ++i){

            int fd = eventsQueue[i].data.fd;

            if(fd == this->getServerFd()){
                int clientFd = this->acceptClient();
                this->addClientToEpool(clientFd);
            }
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
