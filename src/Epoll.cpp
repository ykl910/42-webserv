#include "../include/Epoll.hpp"
#include "../include/WebServ.hpp"

const int&    Epoll::getEpollFd(void) const {
    return this->_epollFd;
}

int Epoll::acceptClient() {

    std::cout << "New pending connexion..." << std::endl;

    struct sockaddr clientAddr;
    socklen_t clientAddrSize;

    clientAddrSize = sizeof(clientAddr);

    int clientFd = accept(this->getServerFd(), &clientAddr, &clientAddrSize);
    if (clientFd == -1)
        this->printErrorAndThrow("accept");

    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
        this->printErrorAndThrow("fcntl");

    std::cout << "Accepted from address: " << clientAddr.sa_data << std::endl;

    return clientFd;
}

void Epoll::createEpollInstance(){

    std::cout << "Creating new epool instance" << std::endl;

    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1)
        this->printErrorAndThrow("epoll_create1");
}

void Epoll::addServerToEpool(){

    std::cout << "Adding server to epoll instance" << std::endl;

    epoll_ev server_ev;
    server_ev.events = EPOLLIN | EPOLLET;
    server_ev.data.fd = this->getServerFd();

    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->getServerFd(), &server_ev) == -1)
        this->printErrorAndThrow("epoll_ctl");
}

void Epoll::addClientToEpool(int const &clientFd){

    std::cout << "Adding client to epoll instance" << std::endl;

    epoll_ev client_ev;
    client_ev.events = EPOLLIN | EPOLLET;
    client_ev.data.fd = clientFd;

    if (epoll_ctl(this->getEpollFd(), EPOLL_CTL_ADD, clientFd, &client_ev) == -1)
        this->printErrorAndThrow("epoll_ctl");
}

bool Epoll::receivedCompleteRequest(std::string &rawData) const {
    //TODO : check if it's a non POST request, otherwise check the content-lenght
    return rawData.find("\r\n\r\n") != std::string::npos;
}


void Epoll::HttpRequestAndResponse(int &clientFd){

    char buffer[BUFFERSIZE];

    int bytes = recv(clientFd, buffer, sizeof(buffer), 0);
    if(bytes > 0)
        this->_buffers[clientFd].append(buffer, bytes);

    if(receivedCompleteRequest(this->_buffers[clientFd])){

        HttpRequest request(_buffers[clientFd]);
        this->sendHttpResponse(clientFd, request);
        _buffers.erase(clientFd);
    }
}

void Epoll::sendHttpResponse(int &clientFd, HttpRequest &request)
{
    HttpResponse Response(request);
    size_t totalBytesSent = 0;
    std::string response = Response.getResponse();
    size_t responseLen = response.length();

    while (totalBytesSent < responseLen){
        ssize_t bytesSent = send(clientFd, response.c_str() + totalBytesSent, responseLen - totalBytesSent, 0);
        if (bytesSent == -1)
            this->printErrorAndThrow("send");
        totalBytesSent += bytesSent;
    }
}

void Epoll::eventManager(epoll_ev &event){

    if(event.events & EPOLLERR){

        int err = 0;
        socklen_t len = sizeof(err);

        if(getsockopt(event.data.fd, SOL_SOCKET, SO_ERROR, &err, &len) == -1)
            this->printError();

        close(event.data.fd);
    }
    if(event.events & EPOLLHUP){

        std::cout << "Connexion closed with a client" << std::endl;
        close(event.data.fd);
    }
    if(!(event.events & EPOLLIN)){

        std::cout << "Strange client behavoir..." << std::endl;
    }
    if((event.events & EPOLLIN) && event.data.fd == this->getServerFd()){

        int clientFd = this->acceptClient();
        this->addClientToEpool(clientFd);
    }
    else if(event.events & EPOLLIN){

        this->HttpRequestAndResponse(event.data.fd);
    }
}

void Epoll::run(WebServ<Epoll>& server) {

    server.printServerStatus("epoll");

    this->createEpollInstance();
    this->addServerToEpool();

    vector eventsQueue;

    while(true) {

        int nbEvents = epoll_wait(this->_epollFd, eventsQueue.data(), MAXEVENTS, -1);
        if(nbEvents == -1)
            this->printErrorAndThrow("epoll_wait");
        for(int i = 0; i < nbEvents; ++i){
            this->eventManager(eventsQueue[i]);
        }
    }
    close(this->_epollFd);
}

Epoll::Epoll() {

}

Epoll::~Epoll() {
    if (this->_epollFd)
        close(this->_epollFd);

}
