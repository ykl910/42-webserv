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

std::vector<Server> Epoll::getServer(void) const {
    return _server;
}

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

    } else if (event.events & EPOLLIN) {
        bool isServerSocket = false;
        for (serverIterator it = _server.begin(); it != _server.end(); ++it) {
            if (event.data.fd == it->getSocketFd()) {
                int clientFd = it->getSocket().acceptClient();
                if (clientFd)
                    addClientToEpoll(clientFd, *it);
                isServerSocket = true;
                break;
            }
        }

        if (!isServerSocket) {
            Server serv = _clientToServer[event.data.fd];

            HttpManager(event.data.fd, serv.getServerAttribute());
            _clientToServer.erase(event.data.fd);
            epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
            close(event.data.fd);
        }
    }
}

void Epoll::addClientToEpoll(int const &clientFd, Server serv)
{
    epoll_ev newClient;
    newClient.events = EPOLLIN;
    newClient.data.fd = clientFd;

    _clientToServer[clientFd] = serv;
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

void    Epoll::initServer(Config& config)
{
    int i = 0;
    configParser parser = config.getConfigParser();
    for (configParserIterator it = parser.begin();
                              it != parser.end(); ++it) {
        server config = *it;
        _server.push_back(Server(config));
        _server[i].initSocket();
        ++i;
    }
}

Epoll::Epoll(Config& config) : _eventsQueue(MAXEVENTS)
{
    _epollFd = epoll_create(1);
    if (_epollFd == -1)
        printErrorAndThrow("epoll_create");

    initServer(config);
    for (serverIterator it = _server.begin(); it != _server.end(); ++it) {
        epoll_ev server_ev;
        server_ev.events = EPOLLIN;
        server_ev.data.fd = it->getSocketFd();
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
