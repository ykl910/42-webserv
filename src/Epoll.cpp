#include "../include/Epoll.hpp"
#include "../include/WebServ.hpp"

const int&    Epoll::getEpollFd(void) const {
    return this->_epollFd;
}

void Epoll::enableWriteEvent(int clientFd) {

    epoll_ev ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = clientFd;
    if(epoll_ctl(this->getEpollFd(), EPOLL_CTL_MOD, clientFd, &ev) == -1)
        printErrorAndThrow("epoll_ctl(enableWriteEvent)");
}

void Epoll::disableWriteEvent(int clientFd) {

    epoll_ev ev;
    ev.events = EPOLLIN;
    ev.data.fd = clientFd;
    if(epoll_ctl(this->getEpollFd(), EPOLL_CTL_MOD, clientFd, &ev) == -1)
        printErrorAndThrow("epoll_ctl(enableWriteEvent)");
}

int Epoll::acceptClient() {

    std::cout << "New pending connexion..." << std::endl;

    struct sockaddr clientAddr;
    socklen_t clientAddrSize;

    clientAddrSize = sizeof(clientAddr);

    int clientFd = accept(this->getSocketFd(), &clientAddr, &clientAddrSize);
    if (clientFd == -1)
        printErrorAndThrow("accept");

    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
        printErrorAndThrow("fcntl");

    return clientFd;
}

void Epoll::createEpollInstance() {

    std::cout << "Creating new epool instance" << std::endl;

    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1)
        printErrorAndThrow("epoll_create1");
}

void Epoll::addServerToEpoll() {

    std::cout << "Adding server to epoll instance" << std::endl;

    epoll_ev server_ev;
    server_ev.events = EPOLLIN;
    server_ev.data.fd = this->getSocketFd();

    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->getSocketFd(), &server_ev) == -1)
        printErrorAndThrow("epoll_ctl");
}

void Epoll::addClientToEpoll(int const &clientFd) {

    std::cout << "Adding client to epoll instance" << std::endl;

    epoll_ev client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = clientFd;

    if (epoll_ctl(this->getEpollFd(), EPOLL_CTL_ADD, clientFd, &client_ev) == -1)
        printErrorAndThrow("epoll_ctl");
}

bool Epoll::receivedCompleteRequest(std::string &rawData) const {

    size_t headerEnd = rawData.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;

    size_t bodyStart = headerEnd + 4;

    size_t contentLengthPos = rawData.find("Content-Length: ");
    if (contentLengthPos == std::string::npos)
        return true;

    size_t valueStart = contentLengthPos + strlen("Content-Length: ");
    size_t valueEnd = rawData.find("\r\n", valueStart);
    std::string valueStr = rawData.substr(valueStart, valueEnd - valueStart);
    int contentLength = std::atoi(valueStr.c_str());

    size_t bodyLengh = rawData.size() - bodyStart;
    return (bodyLengh >= static_cast<size_t>(contentLength));
}

void Epoll::getRequest(int clientFd) {

    char buffer[BUFFERSIZE];

    int bytes = recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytes > 0)
        this->_buffers[clientFd].append(buffer, bytes);

    if (receivedCompleteRequest(this->_buffers[clientFd])) {

        std::cout << BOLD ITALIC GREEN << "\nreceived:\n" << DEFAULT;
        std::cout << MAGENTA << this->_buffers[clientFd] << DEFAULT << std::endl;
        HttpRequest request(_buffers[clientFd]);
        this->_requests[clientFd] = request;
        this->enableWriteEvent(clientFd);
        _buffers.erase(clientFd);
    }
}

void Epoll::sendResponse(int clientFd, HttpRequest request) {
    HttpResponse Response(request);
    size_t totalBytesSent = 0;
    std::string response = Response.getResponse();
    size_t responseLen = response.length();

    while (totalBytesSent < responseLen) {
        ssize_t bytesSent = send(clientFd, response.c_str() + totalBytesSent, responseLen - totalBytesSent, 0);
        if (bytesSent == -1)
            printErrorAndThrow("send");
        totalBytesSent += bytesSent;
    }
    this->disableWriteEvent(clientFd);
}

void Epoll::eventManager(epoll_ev &event) {

    if (event.events & EPOLLERR){

        int err = 0;
        socklen_t len = sizeof(err);

        if (getsockopt(event.data.fd, SOL_SOCKET, SO_ERROR, &err, &len) == -1)
            printError();
        this->_buffers.erase(event.data.fd);
        epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
        close(event.data.fd);
    }
    else if (event.events & EPOLLHUP){

        std::cout << "Connexion closed with a client" << std::endl;
        this->_buffers.erase(event.data.fd);
        epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
        close(event.data.fd);
    }
    else if ((event.events & EPOLLIN) && event.data.fd == this->getSocketFd()){

        int clientFd = this->acceptClient();
        this->addClientToEpoll(clientFd);
    }
    else if(event.events & EPOLLIN)
        this->getRequest(event.data.fd);
    else if(event.events & EPOLLOUT)
        this->sendResponse(event.data.fd, this->_requests[event.data.fd]);
}

void Epoll::run(WebServ<Epoll>& server) {

    server.printServerStatus("epoll");

    this->createEpollInstance();
    this->addServerToEpoll();

    while(true) {
        int nbEvents = epoll_wait(this->_epollFd, this->_eventsQueue.data(), MAXEVENTS, -1);
        if (nbEvents == -1)
            printErrorAndThrow("epoll_wait");
        for (int i = 0; i < nbEvents; ++i)
            this->eventManager(this->_eventsQueue[i]);
    }
}

Epoll::Epoll() : _eventsQueue(MAXEVENTS) {

}

Epoll::~Epoll() {
    if (this->_epollFd)
        close(this->_epollFd);
    // for (buffersIt it = this->_buffers.begin(); it != this->_buffers.end(); ++it)
    //     close(it->first);
}
