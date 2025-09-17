#include "../../include/Poll.hpp"
#include "../../include/HttpManager.hpp"
#include "../../include/WebServ.hpp"

std::vector<Server> Poll::getServer(void) const {
    return _server;
}

inline bool Poll::isSocketFd(int fd) const
{
    std::map<int, Server>::const_iterator it = _serverMap.find(fd);
    return it != _serverMap.end();
}

void    Poll::addClientToPoll(int clientFd, int serverFd)
{
    struct pollfd newClient;

    newClient.fd = clientFd;
    newClient.events = POLLIN | POLLHUP | POLLERR;
    newClient.revents = 0;

    _newClientFd.push_back(newClient);
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
    std::cout
    << RED << "client [" << it->fd << "]: Connection closed"
    << DEFAULT << '\n';

    close(it->fd);
    _clientMap.erase(it->fd);
    _pollFdMap.erase(it->fd);
    _persistance.erase(it->fd);
    _clientState.erase(it->fd);
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

        errno = 0;
        _activity = poll(&_pollFd[0], _pollFd.size(), -1);
        if (g_signal == SIGINT)
            return;
        else if (_activity == -1)
            printError();

        for (pollIterator it = _pollFd.begin();
                          it != _pollFd.end();) {
            if (it->revents & POLLIN) {
                if (isSocketFd(it->fd)) {
                    std::cout << it->fd << '\n';
                    int clientFd = _serverMap[it->fd].getSocket().acceptClient();

                    if (clientFd)
                        addClientToPoll(clientFd, it->fd);
                    ++it;
                } else {
                    HttpManager(it->fd,
                                _clientMap[it->fd],
                                _serverMap,
                                _clientState[it->fd],
                                _persistance[it->fd]);

                    if (_clientState[it->fd] == RESPONSE_TRUNCATE)
                        enableWriteEvent(it->fd);
                    if (_clientState[it->fd] == SENT)
                    {
                        if (!_persistance[it->fd]) {
                            removeClientFromPoll(it);
                            continue;
                        }
                        else
                            _clientState[it->fd] = PENDING;
                    }
                    ++it;
                }

            } else if (it->revents & POLLOUT) {
                    HttpManager(it->fd,
                                _clientMap[it->fd],
                                _serverMap,
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
                        ++it;
                    }
                } else
                    ++it;

            } else if (it->revents & POLLERR) {
                std::cout << "Poll: error catched for client "
                << it->fd << "\n";
                removeClientFromPoll(it);
                continue;

            } else if (it->revents & POLLHUP) {
                std::cout << "Poll: connexion closed for client "
                << it->fd << "\n";
                removeClientFromPoll(it);
                continue;

            } else
                ++it;
        }

        if (!_newClientFd.empty()) {
            _pollFd.insert(_pollFd.end(), _newClientFd.begin(), _newClientFd.end());
            _newClientFd.clear();
        }
    }
}

void    Poll::findSocketPort(Socket& socketReference,
                               std::vector<Server>& servers, std::string port)
{
    for (serverIterator it = servers.begin(); it != servers.end(); ++it) {
       if (it->getServerAttribute().port == port) {
            socketReference = it->getSocket();
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
            config.getLocationNbr(i), config.getCgiNbr(i)));

        std::string port(_server[i].getServerAttribute().port);

        if (!_server[i].getSocket().portAlreadyUsed(port)) {
            _server[i].initSocket();
            _server[i].getServerAttribute().defaultHost = true;
            fd = _server[i].getSocketFd();
            _pollFd.push_back((pollfd){fd, POLLIN | POLLERR, 0});

        } else {
            Socket socketReference;

            findSocketPort(socketReference, _server, port);
            _server[i].getServerAttribute().defaultHost = false;
        }

        _serverMap.insert(
            std::pair<int, Server>(_server[i].getSocketFd(), _server[i]));
        ++i;
    }
}

Poll::Poll(Config& config) : _activity(0)
{
    _newClientFd.clear();
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
