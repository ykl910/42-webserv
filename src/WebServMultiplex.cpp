#include "../include/WebServ.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/HttpResponse.hpp"

bool WebServ::receivedCompleteRequest(std::string &rawData) const {
    //TODO : check if it's a non POST request, otherwise check the content-lenght
    return rawData.find("\r\n\r\n") != std::string::npos;
}

void WebServ::addServerToEpoll(){

    struct epoll_event server_ev;
    server_ev.events = EPOLLIN;
    server_ev.data.fd = this->_serverFd;

    if(epoll_ctl(this->_epollFd, EPOLL_CTL_ADD,this->_serverFd, &server_ev) == -1)
        printErrorAndThrow("epoll_ctl");
}

void WebServ::acceptClient(){

    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize;

    clientAddrSize = sizeof(clientAddr);

    int clientFd = accept(this->_serverFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrSize);
    if(clientFd == -1)
        printErrorAndThrow("accept");

    struct epoll_event client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = clientFd;

    if(epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, clientFd, &client_ev) == -1)
        printErrorAndThrow("epoll_ctl");

}

HttpRequest WebServ::receiveHttpRequest(int &clientFd)
{
    int bytesReceived = 0;
    char buffer[BUFFERSIZE];
    std::string rawData;

    while (!receivedCompleteRequest(rawData)){
        bytesReceived = recv(clientFd, buffer, sizeof(buffer), 0);
        if(bytesReceived == -1)
            printErrorAndThrow("recv");
        rawData.append(buffer, bytesReceived);
    }
    close(clientFd);
    HttpRequest request(rawData);

    return request;
}

void WebServ::sendHttpResponse(int &clientFd, HttpRequest &request)
{
    HttpResponse Response(request);
    size_t totalBytesSent = 0;
    std::string response = Response.getResponse();
    size_t responseLen = response.length();

    while(totalBytesSent < responseLen){
        ssize_t bytesSent = send(clientFd, response.c_str() + totalBytesSent, responseLen - totalBytesSent, 0);
        if(bytesSent == -1)
            printErrorAndThrow("send");
        totalBytesSent += bytesSent;
    }
}

void    WebServ::multiplexEpoll() {

    std::vector<struct epoll_event> events(4096); //!This server is not for pussies

    this->_epollFd = epoll_create1(0);
    if(this->_epollFd == -1)
        printErrorAndThrow("epoll_create1");

    addServerToEpoll();

    while(true){

        int nbEvents = epoll_wait(_epollFd, events.data(), events.size(), -1);
        if(nbEvents == -1)
            printErrorAndThrow("epoll_wait");
        for(int i = 0; i < nbEvents; ++i){

            int fd = events[i].data.fd;

            if(fd == this->_serverFd)
                acceptClient();
           // else
                //Do some shit
        }

    }


    // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    close(this->_epollFd);
}


void    WebServ::multiplexSelect() {
    fd_set readFds;
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    int maxFd = this->_serverFd;

    while (true) {

        //* FD_ZERO = empty readFds set
        //* FD_SET = add server socket to detect new connexions
        FD_ZERO(&readFds);
        FD_SET(this->_serverFd, &readFds);

        //* Add everyt client sockets to readFds and add maxFd if necessary
        for (fdsIterator it = this->_clientFds.begin();
            it != this->_clientFds.end(); ++it) {
            FD_SET(*it, &readFds);
            if (*it > maxFd)
                maxFd = *it;
        }

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        //* wait for event in a socket
        errno = 0;
        int activity = select(maxFd + 1, &readFds, NULL, NULL, &tv);
        if (activity < 0) {
            printError();
            continue;
        }

        //* new connexion -> accept connexion and add client to the list
        if (FD_ISSET(this->_serverFd, &readFds)) {
            errno = 0;
            int newClient = accept(this->_serverFd, NULL, NULL);
            if (newClient < 0) {
                printError();
                continue;
            } else {
                this->_clientFds.push_back(newClient);
                std::cout << "New client connected: FD " << newClient << std::endl;
            }
        }

        //* loop on every actives clients and seek for data to read
        for (fdsIterator it = this->_clientFds.begin(); it != this->_clientFds.end();) {
            char buf[4096];
            int bytes = 0;

            if (FD_ISSET(*it, &readFds)) {
                bytes = recv(*it, buf, sizeof(buf), 0);
                if (bytes <= 0) {
                    close(*it);
                    std::cout << "Client disconnected: FD " << *it << std::endl;
                    it = this->_clientFds.erase(it);
                    continue;
                }

                std::string request(buf, bytes);
                HttpRequest httpReq(request);
                std::cout << "Received request:\n" << request << std::endl;
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello from webserv!</h1></body></html>";
                send(*it, response.c_str(), response.size(), 0);
                close(*it);
                it = this->_clientFds.erase(it);
            }
            else
                ++it;
        }
    }
}

void    WebServ::multiplexPoll() {}


