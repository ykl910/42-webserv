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

void Epoll::addClientToEpoll(int clientFd, int serverFd)
{
    epoll_ev newClient;
    newClient.events = EPOLLIN | EPOLLRDHUP;
    newClient.data.fd = clientFd;

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &newClient) == -1) {
        printErrorAndThrow("epoll_ctl");
        close(clientFd);
    } else {
        _clientMap.insert(
            std::pair<int, int>(clientFd, _serverMap[serverFd].getSocketFd()));

        std::cout << BOLD WHITE << "Epoll: new client accepted with fd "
        BOLD BLUE << newClient.data.fd << DEFAULT << "\n";
        _clientState[clientFd] = PENDING;
    }
}

void Epoll::enableWriteEvent(int clientFd)
{
    std::cout << GREEN << "Write event enabled" << DEFAULT << std::endl;
    epoll_ev ev;
    ev.events = EPOLLIN | EPOLLRDHUP| EPOLLOUT;
    ev.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1)
        printErrorAndThrow("epoll_ctl(enableWriteEvent)");
}

void Epoll::disableWriteEvent(int clientFd)
{
    epoll_ev ev;
    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1)
        printErrorAndThrow("epoll_ctl(enableWriteEvent)");
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
    if (event.events & EPOLLERR) {
        int err = 0;
        socklen_t len = sizeof(err);

        if (getsockopt(event.data.fd, SOL_SOCKET, SO_ERROR, &err, &len) == -1)
            printError();
        _buffers.erase(event.data.fd);
        epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
        close(event.data.fd);

    } else if (event.events & EPOLLHUP || event.events & EPOLLRDHUP) {
        std::cout << "Connexion closed with a client" << std::endl;
        _buffers.erase(event.data.fd);
        epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
        close(event.data.fd);

    } else if (event.events & EPOLLIN || event.events & EPOLLOUT) {

        if (isSocketFd(event.data.fd)) {
            int clientFd = _serverMap[event.data.fd].getSocket().acceptClient();
            if (clientFd)
                addClientToEpoll(clientFd, event.data.fd);
        } else {
            Server serv = _serverMap[_clientMap[event.data.fd]];

            HttpManager(event.data.fd, serv.getServerAttribute(), _clientState[event.data.fd]);
            if (_clientState[event.data.fd] == RESPONSE_TRUNCATE)
                enableWriteEvent(event.data.fd);
            if (_clientState[event.data.fd] == SENT) {
                disableWriteEvent(event.data.fd);
                //std::cout << RED BOLD << "client: " << event.data.fd << " erased from epoll" << DEFAULT << std::endl;
                //epoll_ctl(_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
                //_clientMap.erase(event.data.fd);
                //close(event.data.fd);
            }
        }
    }
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
    int fd;
    int i = 0;

    configParser parser = config.getConfigParser();
    for (configParserIterator it = parser.begin();
                              it != parser.end(); ++it) {
        server config = *it;
        _server.push_back(Server(config));
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
    for (buffersIt it = _buffers.begin(); it != _buffers.end(); ++it) {
        if (it->first)
            close(it->first);
    }
}
