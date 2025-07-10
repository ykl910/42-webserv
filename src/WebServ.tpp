#include "../include/WebServ.hpp"

template <class Multiplexer>
bool WebServ<Multiplexer>::receivedCompleteRequest(std::string &rawData) const {
    //TODO : check if it's a non POST request, otherwise check the content-lenght
    return rawData.find("\r\n\r\n") != std::string::npos;
}

template <class Multiplexer>
HttpRequest WebServ<Multiplexer>::receiveHttpRequest(int &clientFd)
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

template <class Multiplexer>
void WebServ<Multiplexer>::sendHttpResponse(int &clientFd, HttpRequest &request)
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

template <class Multiplexer>
void WebServ<Multiplexer>::printServerStatus(const char* multiplexer) const {
    std::cout << BOLD WHITE << "Server status: "
    << BOLD ITALIC GREEN << "running\n" << DEFAULT
    << BOLD WHITE << "Port: "
    << BOLD ITALIC BLUE  << "8080\n" << DEFAULT
    << BOLD WHITE << "Multiplexer: "
    << BOLD ITALIC BLUE  << multiplexer << DEFAULT
    << std::endl;
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ() {
    this->runMultiplexer();
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ(const char* configFile) {}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
