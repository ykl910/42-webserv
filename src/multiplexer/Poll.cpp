#include "../../include/Poll.hpp"
#include "../../include/HttpManager.hpp"
#include "../../include/WebServ.hpp"

/*
    struct pollfd {
        int   fd;         file descriptor
        short events;     requested events
        short revents;    returned events
    };
*/

std::vector<Server> Poll::getServer(void) const {
    return _server;
}

inline bool Poll::isSocketFd(int fd) const
{
    for (size_t i = 0; i < _listenFd.size(); ++i) {
        if (fd == _listenFd[i])
            return true;
    }
    return false;
}

void    Poll::addClientToPoll(int clientFd, int serverFd)
{
    struct pollfd newClient;

    newClient.fd = clientFd;
    newClient.events = POLLIN | POLLHUP | POLLERR;
    newClient.revents = 0;

    _pollFd.push_back(newClient);
    _pollFdMap.insert(
        std::pair<int, struct pollfd>(clientFd, newClient));
    _clientMap.insert(
        std::pair<int, int>(clientFd, _serverMap[serverFd].getSocketFd()));

    std::cout << BOLD WHITE << "Poll: new client accepted with fd "
    << BOLD BLUE << clientFd << DEFAULT << "\n";
    _clientState[clientFd] = PENDING;
    _persistance[clientFd] = false;
}

void    Poll::removeClientFromPoll(pollIterator& it)
{
    _clientMap.erase(it->fd);
    close(it->fd);
    _persistance.erase(it->fd);
    _clientState.erase(it->fd);
    _pollFdMap.erase(it->fd);
    it = _pollFd.erase(it);
}

void    Poll::enableWriteEvent(int clientFd)
{
    std::cout
    << GREEN << "client [" << clientFd << "]: Write event enabled"
    << DEFAULT << '\n';

    _pollFdMap[clientFd].events = POLLIN | POLLRDHUP | POLLERR | POLLOUT;
}

void    Poll::disableWriteEvent(int clientFd)
{
    std::cout
    << GREEN << "client [" << clientFd << "]: Write event disabled"
    << DEFAULT << '\n';

    _pollFdMap[clientFd].events = POLLIN | POLLRDHUP | POLLERR;
}

void    Poll::run()
{
    while (g_signal != SIGINT) {
        _activity = poll(&_pollFd[0], _pollFd.size(), 10); // 10 ms timeout
        if (g_signal == SIGINT)
            return;
        else if (_activity == -1)
            printError();

        for (size_t i = 0; i < _listenFd.size(); ++i) {

            if (_pollFd[i].revents & POLLERR) {
                std::cout << "Poll: error catched from server fd.\n";
            } else if (_pollFd[i].revents & POLLIN) {
                int clientFd = _server[i].getSocket().acceptClient();

                if (clientFd)
                    addClientToPoll(clientFd, _listenFd[i]);
            }
        }

        for (pollIterator it = _pollFd.begin() + _listenFd.size(); // skip socket fd
                          it != _pollFd.end();) {
            if (it->revents & POLLERR) {
                std::cout << "Poll: error catched for client "
                << it->fd << "\n";

            } else if (it->revents & POLLHUP) {
                std::cout << "Poll: connexion closed for client "
                << it->fd << "\n";
                _clientMap.erase(it->fd);
                close(it->fd);
                it = _pollFd.erase(it);

            } else if (it->revents & POLLIN) {
                Server serv = _serverMap[_clientMap[it->fd]];
                HttpManager(it->fd,
                            serv.getServerAttribute(),
                            _clientState[it->fd],
                            _persistance[it->fd]);

                if (_clientState[it->fd] == RESPONSE_TRUNCATE)
                    enableWriteEvent(it->fd);
                if (_clientState[it->fd] == SENT)
                {
                    if (!_persistance[it->fd])
                        removeClientFromPoll(it);
                    else
                        _clientState[it->fd] = PENDING;
                }

            } else if (it->revents & POLLOUT) {
                Server serv = _serverMap[_clientMap[it->fd]];
                HttpManager(it->fd,
                            serv.getServerAttribute(),
                            _clientState[it->fd],
                            _persistance[it->fd]);

                if (_clientState[it->fd] == SENT)
                {
                    if (!_persistance[it->fd])
                        removeClientFromPoll(it);
                    else
                    {
                        disableWriteEvent(it->fd);
                        _clientState[it->fd] = PENDING;
                    }
                }
            } else
                ++it;
        }
    }
}

void    Poll::initServer(Config& config)
{
    int fd;
    int i = 0;

    configParser parser = config.getConfigParser();
    for (configParserIterator it = parser.begin();
                              it != parser.end(); ++it) {
        server serverConfig = *it;
        _server.push_back(Server(serverConfig,
            config.getLocationNbr(i), config.getCgiNbr(i), config.getCgiTotal()));
        _server[i].initSocket();
        _serverMap.insert(
            std::pair<int, Server>(_server[i].getSocketFd(), _server[i]));
        fd = _server[i].getSocketFd();
        _listenFd.push_back(fd);
        _pollFd.push_back((pollfd){fd, POLLIN | POLLERR, 0});
        ++i;
    }
}

Poll::Poll(Config& config)
{
    initServer(config);
}

Poll::~Poll()
{
    for (pollIterator it = _pollFd.begin();
                      it != _pollFd.end(); it++) {
        if (it->fd)
            close(it->fd);
    }
}
