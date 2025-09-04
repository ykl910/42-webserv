#include "../include/Poll.hpp"
#include "../include/HttpManager.hpp"
#include "../include/WebServ.hpp"

/*
    struct pollfd {
        int   fd;         file descriptor
        short events;     requested events
        short revents;    returned events
    };
*/

bool    Poll::isSocketFd(int fd) const
{
    for (size_t i = 0; i < _listenFd.size(); ++i) {
        if (fd == _listenFd[i])
            return true;
    }
    return false;
}

void    Poll::addClientToPoll(int clientFd, Server serv)
{
    _pollFd.push_back((pollfd){clientFd, POLLIN, 0});
    _serverFinder[clientFd] = serv;
    std::cout << BOLD WHITE << "Poll: new client accepted with fd "
    << BOLD BLUE << clientFd << DEFAULT << "\n";
}

void    Poll::handleNewConnexion(struct pollfd& server, int i)
{
    if (server.revents & POLLERR)
        std::cout << "Poll: error catched from server fd.\n";
    else if (server.revents & POLLIN) {
        Server serv;
        for(serverIterator it = _server.begin(); it != _server.end(); ++it) {
            if (it->getSocketFd() == server.fd) {
                serv = *it;
                break;
            }
        }
        int clientFd = _server[i].getSocket().acceptClient();
        if (clientFd)
            addClientToPoll(clientFd, serv);
    }
}

std::vector<Server> Poll::getServer(void) const {
    return _server;
}

void    Poll::run()
{
    while (true) {
        _activity = poll(&_pollFd[0], _pollFd.size(), 10); // 10 ms timeout
        if (_activity == -1) {
            if (g_signal == SIGINT)
                return;
            printError();
        }

        size_t i = 0;
        while (i < _pollFd.size() && isSocketFd(_pollFd[i].fd)) {
            handleNewConnexion(_pollFd[i], i);
            i++;
        }

        for (pollIterator it = _pollFd.begin() + _listenFd.size(); // skip socket fd
                          it != _pollFd.end();) {
            if (it->revents & POLLERR)
                std::cout << "Poll: error catched for client "
                << it->fd << "\n";
            else if (it->revents & POLLHUP) {
                std::cout << "Poll: connexion closed for client "
                << it->fd << "\n";
                _serverFinder.erase(it->fd);
                close(it->fd);
                it = _pollFd.erase(it);
            } else if (it->revents & POLLIN) {
                Server serv = _serverFinder[it->fd];

                HttpManager(it->fd, serv.getServerAttribute(), _state);
                _serverFinder.erase(it->fd);
                close(it->fd);
                it = _pollFd.erase(it);
            } else if (it->revents & POLLOUT) {
                Server serv = _serverFinder[it->fd];

                _state = OUT;
                HttpManager(it->fd, serv.getServerAttribute(), _state);
                _serverFinder.erase(it->fd);
                close(it->fd);
                it = _pollFd.erase(it);
            } else
                ++it;
        }
    }
}

void    Poll::initServer(Config& config)
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

Poll::Poll(Config& config)
{
    initServer(config);
    for (serverIterator it = _server.begin(); it != _server.end(); ++it) {
        int fd = it->getSocketFd();
        _listenFd.push_back(fd);
        _pollFd.push_back((pollfd){fd, POLLIN, 0});
    }
}

Poll::~Poll()
{
    for (pollIterator it = _pollFd.begin();
                      it != _pollFd.end(); it++) {
        if (it->fd)
            close(it->fd);
    }
}
