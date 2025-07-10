#include "../include/WebServ.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/HttpResponse.hpp"

bool WebServ::receivedCompleteRequest(std::string &rawData) const {
    //TODO : check if it's a non POST request, otherwise check the content-lenght
    return rawData.find("\r\n\r\n") != std::string::npos;
}

void WebServ::acceptClient() {

    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize;

    clientAddrSize = sizeof(clientAddr);

    int clientFd = accept(this->_socket.getServerFd(), reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrSize);
    if (clientFd == -1)
        this->_error.printErrorAndThrow("accept");

    struct epoll_event client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = clientFd;

    if (epoll_ctl(this->_epoll.getEpollFd(), EPOLL_CTL_ADD, clientFd, &client_ev) == -1)
        this->_error.printErrorAndThrow("epoll_ctl");

}

HttpRequest WebServ::receiveHttpRequest(int &clientFd)
{
    int bytesReceived = 0;
    char buffer[BUFFERSIZE];
    std::string rawData;

    while (!receivedCompleteRequest(rawData)){
        bytesReceived = recv(clientFd, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1)
            this->_error.printErrorAndThrow("recv");
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

    while (totalBytesSent < responseLen){
        ssize_t bytesSent = send(clientFd, response.c_str() + totalBytesSent, responseLen - totalBytesSent, 0);
        if (bytesSent == -1)
            this->_error.printErrorAndThrow("send");
        totalBytesSent += bytesSent;
    }
}
