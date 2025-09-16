#include "../../include/Epoll.hpp"
#include "../../include/HttpManager.hpp"
#include "../../include/WebServ.hpp"

std::vector<Server> Epoll::getServer(void) const {
    return _server;
}

void Epoll::printFdError(int clientFd)
{
    int err = 0;
    socklen_t len = sizeof(err);

    if (getsockopt(clientFd, SOL_SOCKET, SO_ERROR, &err, &len) == -1)
        printError();
}

void Epoll::addClientToEpoll(int clientFd, int serverFd)
{
    epoll_ev newClient;
    newClient.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
    newClient.data.fd = clientFd;

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &newClient) == -1) {
        printErrorAndThrow("epoll_ctl");
        close(clientFd);
    } else {
        _clientMap.insert(
            std::pair<int, int>(clientFd, _serverMap[serverFd].getSocketFd()));

        std::cout << BOLD WHITE << "Epoll: new client accepted with fd "
        << BOLD BLUE << newClient.data.fd << DEFAULT << std::endl;
        _clientState[clientFd] = PENDING;
        _persistance[clientFd] = false;
    }
}

void Epoll::removeClientFromEpoll(int clientFd)
{
    std::cout
    << RED << "client [" << clientFd << "]: Connection closed"
    << DEFAULT << '\n';

    epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL);
    close(clientFd);
    _persistance.erase(clientFd);
    _clientState.erase(clientFd);
}

void Epoll::enableWriteEvent(int clientFd)
{
    std::cout
    << GREEN << "client [" << clientFd << "]: Write event enabled"
    << DEFAULT << '\n';

    epoll_ev ev;
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
    ev.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1)
        printErrorAndThrow("epoll_ctl(enableWriteEvent)");
}

void Epoll::disableWriteEvent(int clientFd)
{
    std::cout
    << GREEN << "client [" << clientFd << "]: Write event disabled"
    << DEFAULT << '\n';

    epoll_ev ev;
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
    ev.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1)
        printErrorAndThrow("epoll_ctl(disableWriteEvent)");
}

inline bool Epoll::isSocketFd(int fd) const
{
    for (size_t i = 0; i < _listenFd.size(); ++i) {
        if (fd == _listenFd[i])
            return true;
    }
    return false;
}

void Epoll::eventManager(epoll_ev &event)
{
    if (event.events & EPOLLIN)
    {
        if (isSocketFd(event.data.fd))
        {
            int clientFd = _serverMap[event.data.fd].getSocket().acceptClient();
            if (clientFd)
                addClientToEpoll(clientFd, event.data.fd);
        }
        else
        {
            Server serv = _serverMap[_clientMap[event.data.fd]];
            HttpManager(event.data.fd,
                        serv.getServerAttribute(),
                        _clientState[event.data.fd],
                        _persistance[event.data.fd]);

            if (_clientState[event.data.fd] == RESPONSE_TRUNCATE)
                enableWriteEvent(event.data.fd);
            if (_clientState[event.data.fd] == SENT)
            {
                if (!_persistance[event.data.fd])
                    removeClientFromEpoll(event.data.fd);
                else
                    _clientState[event.data.fd] = PENDING;
            }
        }
    }
    else if (event.events & EPOLLOUT)
    {
        Server serv = _serverMap[_clientMap[event.data.fd]];
        HttpManager(event.data.fd,
                    serv.getServerAttribute(),
                    _clientState[event.data.fd],
                    _persistance[event.data.fd]);

        if (_clientState[event.data.fd] == SENT)
        {
            if (!_persistance[event.data.fd])
                removeClientFromEpoll(event.data.fd);
            else
            {
                disableWriteEvent(event.data.fd);
                _clientState[event.data.fd] = PENDING;
            }
        }
    }
    else if (event.events & EPOLLERR)
    {
        printFdError(event.data.fd);
        removeClientFromEpoll(event.data.fd);
    }
    else if (event.events & EPOLLHUP || event.events & EPOLLRDHUP)
    {
        removeClientFromEpoll(event.data.fd);
    }
}

void Epoll::run()
{
    while (g_signal != SIGINT) {
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
    int fd;
    int i = 0;

    configParser parser = config.getConfigParser();
    for (configParserIterator it = parser.begin();
                              it != parser.end(); ++it) {
        server serverConfig = *it;
        _server.push_back(Server(serverConfig,
            config.getLocationNbr(i), config.getCgiNbr(i)));
        _server[i].initSocket();
        _serverMap.insert(
            std::pair<int, Server>(_server[i].getSocketFd(), _server[i]));
        fd = _server[i].getSocketFd();
        _listenFd.push_back(fd);
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
}
