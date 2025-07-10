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
    server_ev.events = EPOLLIN;
    server_ev.data.fd = this->getServerFd();

    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->getServerFd(), &server_ev) == -1)
        this->printErrorAndThrow("epoll_ctl");
}

void Epoll::addClientToEpool(int const &clientFd){

    std::cout << "Adding client to epoll instance" << std::endl;

    epoll_ev client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = clientFd;

    if (epoll_ctl(this->getEpollFd(), EPOLL_CTL_ADD, clientFd, &client_ev) == -1)
        this->printErrorAndThrow("epoll_ctl");
}

bool Epoll::receivedCompleteRequest(std::string &rawData) const {
    //TODO : check if it's a non POST request, otherwise check the content-lenght
    return rawData.find("\r\n\r\n") != std::string::npos;
}

HttpRequest Epoll::receiveHttpRequest(int &clientFd)
{
    int bytesReceived = 0;
    char buffer[BUFFERSIZE];
    std::string rawData;

    while (!receivedCompleteRequest(rawData)){
        bytesReceived = recv(clientFd, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1)
            this->printErrorAndThrow("recv");
        rawData.append(buffer, bytesReceived);
    }
    close(clientFd);
    HttpRequest request(rawData);

    return request;
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

void    Epoll::run(WebServ& server) {

    server.printServerStatus("epoll");

    this->createEpollInstance();
    this->addServerToEpool();

    vector eventsQueue(MAXEVENTS);

    while(true) {

        int nbEvents = epoll_wait(this->_epollFd, eventsQueue.data(), eventsQueue.size(), 0);
        if(nbEvents == -1)
            this->printErrorAndThrow("epoll_wait");
        for(int i = 0; i < nbEvents; ++i){

            int fd = eventsQueue[i].data.fd;

            if(fd == this->getServerFd()){

                int clientFd = this->acceptClient();
                this->addClientToEpool(clientFd);
            }
            else{

                HttpRequest request = this->receiveHttpRequest(fd);
                this->sendHttpResponse(fd, request);
            }
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
