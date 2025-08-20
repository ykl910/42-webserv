#include "../include/Epoll.hpp"
#include "../include/HttpManager.hpp"
#include "../include/WebServ.hpp"

/*
    typedef union epoll_data {
        void        *ptr;
        int          fd;
        uint32_t     u32;
        uint64_t     u64;
    } epoll_data_t;

    struct epoll_event {
        uint32_t     events;      // Epoll events
        epoll_data_t data;        // User data variable
    };
*/

void Epoll::enableWriteEvent(int clientFd)
{
    epoll_ev ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1)
        printErrorAndThrow("epoll_ctl(enableWriteEvent)");
}

void Epoll::disableWriteEvent(int clientFd)
{
    epoll_ev ev;
    ev.events = EPOLLIN;
    ev.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1)
        printErrorAndThrow("epoll_ctl(enableWriteEvent)");
}

bool Epoll::receivedCompleteRequest(std::string &rawData) const
{
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

// void Epoll::getRequest(int clientFd)
// {
//     char buffer[BUFFERSIZE];

//     int bytes = recv(clientFd, buffer, sizeof(buffer), 0);
//     if (bytes > 0)
//         _buffers[clientFd].append(buffer, bytes);

//     if (receivedCompleteRequest(_buffers[clientFd])) {

//         std::cout << BOLD ITALIC GREEN << "\nreceived:\n" << DEFAULT;
//         std::cout << MAGENTA << _buffers[clientFd] << DEFAULT << std::endl;
//         HttpRequest request(_buffers[clientFd]);
//         _requests[clientFd] = request;
//         _gotFullRequest[clientFd] = true;
//         enableWriteEvent(clientFd);
//         _buffers.erase(clientFd);
//         _pendingResponse[clientFd] = 0;
//     } else
//         _gotFullRequest[clientFd] = false;
// }

void Epoll::sendResponse(int clientFd, HttpRequest request)
{
    std::string response;

    if (!_gotResponse[clientFd]) {
        HttpResponse Response(request);
        writeUserInfo(request, Response);
        response = Response.getResponse();
        _responses[clientFd] = Response;
        _gotResponse[clientFd] = true;
    } else
        response = _responses[clientFd].getResponse();

    size_t totalBytesSent = _pendingResponse[clientFd];
    size_t responseLen = response.length();

    while (totalBytesSent < responseLen) {
        ssize_t bytesSent = send(clientFd, response.c_str() + totalBytesSent,
                                 responseLen - totalBytesSent, 0);
        if (bytesSent <= 0)
            break ;
        totalBytesSent += bytesSent;
    }
    if (totalBytesSent != responseLen)
        _pendingResponse[clientFd] = totalBytesSent;
    else {
        std::cout << BOLD ITALIC GREEN << "\nresponse:\n" << DEFAULT;
        std::cout << YELLOW << response.c_str() << std::endl;
        _pendingResponse.erase(clientFd);
        disableWriteEvent(clientFd);
        _gotFullRequest.erase(clientFd);
        _gotResponse.erase(clientFd);
        _responses.erase(clientFd);
    }
}

void Epoll::eventManager(epoll_ev &event)
{
    if (event.events & EPOLLERR) {
        int err = 0;
        socklen_t len = sizeof(err);

        if (getsockopt(event.data.fd, SOL_SOCKET, SO_ERROR, &err, &len) == -1)
            printError();
        _buffers.erase(event.data.fd);
        epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
        close(event.data.fd);

    } else if (event.events & EPOLLHUP) {
        std::cout << "Connexion closed with a client" << std::endl;
        _buffers.erase(event.data.fd);
        epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
        close(event.data.fd);

    } else if ((event.events & EPOLLIN)
        && event.data.fd == _server[0].socket.getSocketFd()) {
        int clientFd = _server[0].socket.acceptClient();
        if (clientFd)
            addClientToEpoll(clientFd);

    } else if (event.events & EPOLLIN)
        HttpManager(event.data.fd);
        // getRequest(event.data.fd);
    // else if ((event.events & EPOLLOUT) && _gotFullRequest[event.data.fd])
    //     sendResponse(event.data.fd, _requests[event.data.fd]);
}

void Epoll::addClientToEpoll(int const &clientFd)
{
    epoll_ev newClient;

    newClient.events = EPOLLIN;
    newClient.data.fd = clientFd;
    std::cout << BOLD WHITE << "Epoll: new client accepted with fd "
    BOLD BLUE << newClient.data.fd << DEFAULT << "\n";

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &newClient) == -1)
        printErrorAndThrow("epoll_ctl");
}

void Epoll::run()
{
    while (true) {
        _nbEvents = epoll_wait(_epollFd, _eventsQueue.data(), MAXEVENTS, 0);
        if (g_signal == SIGINT)
            return;
        else if (_nbEvents == -1)
            printErrorAndThrow("epoll_wait");
        for (int i = 0; i < _nbEvents; ++i)
            eventManager(_eventsQueue[i]);
    }
}

Epoll::Epoll(config& server) : _eventsQueue(MAXEVENTS)
{
    _server = server;
    _epollFd = epoll_create(1);
    if (_epollFd == -1)
        printErrorAndThrow("epoll_create");

    for (configIterator it = _server.begin(); it != _server.end(); ++it) {
        epoll_ev server_ev;
        memset(&server_ev, 0, sizeof(server_ev));
        server_ev.events = EPOLLIN;
        server_ev.data.fd = it->socket.getSocketFd();
        if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, server_ev.data.fd, &server_ev) == -1)
            printErrorAndThrow("epoll_ctl");
    }
}

Epoll::~Epoll()
{
    if (_epollFd)
        close(_epollFd);
    for (buffersIt it = _buffers.begin(); it != _buffers.end(); ++it) {
        if (it->first)
            close(it->first);
    }
}
