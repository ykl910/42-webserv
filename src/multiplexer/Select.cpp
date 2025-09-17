#include "../../include/Select.hpp"

inline bool    Select::isSocketFd(int fd) const
{
    std::vector<int>::const_iterator it
        = std::find(_listenFd.begin(), _listenFd.end(), fd);
    return it != _listenFd.end();
}

std::vector<Server> Select::getServer(void) const {
    return _server;
}

void    Select::addClientToSelect(int clientFd, int serverFd)
{
    _newClientFd.push_back(clientFd);
    _clientMap.insert(
        std::pair<int, int>(clientFd, _serverMap[serverFd].getSocketFd()));
    std::cout
    << BOLD WHITE << "Select: new client accepted with fd "
    << BOLD BLUE << clientFd << DEFAULT << "\n";
    _clientState[clientFd] = PENDING;
    _persistance[clientFd] = false;
}

void    Select::removeClientFromSelect(int clientFd, size_t& i)
{
    close(clientFd);
    _clientMap.erase(clientFd);
    _persistance.erase(clientFd);
    _clientState.erase(clientFd);
    _clientFd.erase(_clientFd.begin() + i);
    --i;
}

void    Select::run()
{
    while (g_signal != SIGINT) {
        FD_ZERO(&_readFds);
        FD_ZERO(&_writeFds);
        FD_ZERO(&_exceptFds);

        _maxFd = 0;
        for (size_t i = 0; i < _listenFd.size(); ++i) {
            FD_SET(_listenFd[i], &_readFds);
            FD_SET(_listenFd[i], &_exceptFds);
            if (_listenFd[i] > _maxFd)
                _maxFd = _listenFd[i];
        }

        for (size_t i = 0; i < _clientFd.size(); ++i) {
            FD_SET(_clientFd[i], &_readFds);
            FD_SET(_clientFd[i], &_writeFds);
            FD_SET(_clientFd[i], &_exceptFds);
            if (_clientFd[i] > _maxFd)
                _maxFd = _clientFd[i];
        }

        errno = 0;
        _activity = select(_maxFd + 1, &_readFds, &_writeFds, &_exceptFds, NULL);
        if (g_signal == SIGINT)
            return;
        else if (_activity == -1)
            printError();

        for (size_t i = 0; i < _listenFd.size(); ++i) {
            if (FD_ISSET(_listenFd[i], &_readFds)) {
                int clientFd = _serverMap[_listenFd[i]].getSocket().acceptClient();
                if (clientFd)
                    addClientToSelect(clientFd, _listenFd[i]);
            }
            else if (FD_ISSET(_listenFd[i], &_exceptFds))
                std::cout << "Select: error catched for server fd "
                << _listenFd[i] << "\n";
        }

        for (size_t i = 0; i < _clientFd.size();) {
            if (FD_ISSET(_clientFd[i], &_readFds)) {
                    // HttpManager(_clientFd[i],
                    //             _serverMap[_clientMap[_clientFd[i]]].getServerAttribute(),
                    //             _clientState[_clientFd[i]],
                    //             _persistance[_clientFd[i]]);

                    if (_clientState[_clientFd[i]] == SENT)
                    {
                        if (!_persistance[_clientFd[i]])
                            removeClientFromSelect(_clientFd[i], i);
                        else {
                            _clientState[_clientFd[i]] = PENDING;
                            ++i;
                        }
                    } else
                        ++i;
            }

            else if (FD_ISSET(_clientFd[i], &_writeFds)) {
                // HttpManager(_clientFd[i],
                //             _serverMap[_clientMap[_clientFd[i]]].getServerAttribute(),
                //             _clientState[_clientFd[i]],
                //             _persistance[_clientFd[i]]);

                if (_clientState[_clientFd[i]] == SENT)
                {
                    if (!_persistance[_clientFd[i]])
                        removeClientFromSelect(_clientFd[i], i);
                    else
                    {
                        _clientState[_clientFd[i]] = PENDING;
                        ++i;
                    }
                } else
                    ++i;
            }

            else if (FD_ISSET(_clientFd[i], &_exceptFds)) {
                std::cout << "Select: error catched for client fd "
                << _clientFd[i] << "\n";
                removeClientFromSelect(_clientFd[i], i);
            }
            else
                ++i;
        }

        if (!_newClientFd.empty()) {
            _clientFd.insert(_clientFd.end(), _newClientFd.begin(), _newClientFd.end());
            _newClientFd.clear();
        }
    }
}

void    Select::initServer(Config& config)
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

Select::Select(Config& config) : _activity(0)
{
    _newClientFd.clear();

    initServer(config);
}

Select::~Select()
{
    for (selectIterator it = _clientFd.begin();
         it != _clientFd.end(); ++it)
        close(*it);
}
