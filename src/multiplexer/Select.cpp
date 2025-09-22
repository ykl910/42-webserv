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

void    Select::addClientToSelect(int clientFd)
{
    FD_SET(clientFd, &_readFds);
    FD_SET(clientFd, &_exceptFds);

    _clientFd.push_back(clientFd);

    std::cout
    << BOLD WHITE << "Select: new client accepted with fd "
    << BOLD BLUE << clientFd << DEFAULT << "\n";
    _clientState[clientFd] = PENDING;
    _persistance[clientFd] = false;

    if (clientFd > _maxFd)
        _maxFd = clientFd;
}

void    Select::removeClientFromSelect(int clientFd, size_t& i)
{
    std::cout
    << BOLD RED << "client [" << clientFd << "]: Connection closed"
    << DEFAULT << '\n';

    close(clientFd);
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
        _activity = select(_maxFd + 1, &_readFds, &_writeFds, &_exceptFds, &_timeout);
        if (g_signal == SIGINT)
            return;
        else if (_activity == -1)
            printError();

        for (size_t i = 0; i < _listenFd.size(); ++i) {
            if (FD_ISSET(_listenFd[i], &_readFds)) {
                int clientFd = _serverMap[_listenFd[i]].getSocket().acceptClient();
                if (clientFd)
                    addClientToSelect(clientFd);

            } else if (FD_ISSET(_listenFd[i], &_exceptFds))
                std::cout << "Select: error catched for server fd "
                << _listenFd[i] << "\n";
        }

        for (size_t i = 0; i < _clientFd.size();) {

            if (FD_ISSET(_clientFd[i], &_readFds)) {
                    HttpManager(_clientFd[i],
                                _serverMap,
                                _clientState[_clientFd[i]],
                                _persistance[_clientFd[i]]);

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
                HttpManager(_clientFd[i],
                            _serverMap,
                            _clientState[_clientFd[i]],
                            _persistance[_clientFd[i]]);

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

            else if (FD_ISSET(_clientFd[i], &_exceptFds)) {
                std::cout << "Select: error catched for client fd "
                << _clientFd[i] << "\n";
                removeClientFromSelect(_clientFd[i], i);
            }
            else
                ++i;
        }
    }
}

void    Select::findSocketPort(Socket& socketReference,
                               std::vector<Server>& servers, std::string port)
{
    for (serverIterator it = servers.begin(); it != servers.end(); ++it) {
       if (it->getServerAttribute().port == port) {
            socketReference = it->getSocket();
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

        std::string port(_server[i].getServerAttribute().port);

        if (!_server[i].getSocket().portAlreadyUsed(port)) {
            _server[i].initSocket();
            _server[i].getServerAttribute().defaultHost = true;
            fd = _server[i].getSocketFd();
            _listenFd.push_back(fd);

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

Select::Select(Config& config) : _activity(0)
{
    _timeout.tv_sec = 0;
    _timeout.tv_usec = 0;

    initServer(config);
}

Select::~Select()
{
    for (size_t i = 0; i < _listenFd.size(); ++i)
        close(_listenFd[i]);
    for (size_t i = 0; i < _clientFd.size(); ++i)
        close(_clientFd[i]);
}
