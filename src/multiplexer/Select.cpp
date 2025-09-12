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
    _clientFd.push_back(clientFd);
    _clientMap.insert(
        std::pair<int, int>(clientFd, _serverMap[serverFd].getSocketFd()));
    std::cout
    << BOLD WHITE << "Select: new client accepted with fd "
    << BOLD BLUE << clientFd << DEFAULT << "\n";
    _clientState[clientFd] = PENDING;
    _persistance[clientFd] = false;
}

void    Select::removeClientFromSelect(int clientFd, selectIterator& it)
{
    close(clientFd);
    _clientMap.erase(clientFd);
    _persistance.erase(clientFd);
    _clientState.erase(clientFd);
    it = _selectFd.erase(it);
}

void    Select::run()
{
    while (g_signal != SIGINT) {
        FD_ZERO(&_readFds);

        for (size_t i = 0; i < _selectFd.size(); ++i) {
            FD_SET(_selectFd[i], &_readFds);
            if (_selectFd[i] > _maxFd)
                _maxFd = _selectFd[i];
        }

        //* wait for event in a socket
        errno = 0;
        _activity = select(_maxFd + 1, &_readFds, NULL, NULL, &_tv);
        if (g_signal == SIGINT)
            return;
        else if (_activity == -1)
            printError();

        for (selectIterator it = _selectFd.begin(); it != _selectFd.end();) {
            if (FD_ISSET(*it, &_readFds)) {
                if (isSocketFd(*it)) {

                    //* new connexion -> accept connexion and add client to the list
                    int clientFd = _serverMap[*it].getSocket().acceptClient();
                    if (clientFd)
                        addClientToSelect(clientFd, *it);
                    ++it;
                } else { //* read request and send response
                    int serverFd = _clientMap[*it];
                    HttpManager(int(*it),
                                _serverMap[serverFd].getServerAttribute(),
                                _clientState[*it],
                                _persistance[*it]);

                    if (!_persistance[*it])
                        removeClientFromSelect(*it, it);
                    else
                        _clientState[*it] = PENDING;
                }
            } else if (FD_ISSET(*it, &_writeFds)) {
                continue;

            } else
                ++it;
        }
        _selectFd.insert(_selectFd.end(), _clientFd.begin(), _clientFd.end());
        _clientFd.clear();
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
            config.getLocationNbr(i), config.getCgiNbr(i), config.getCgiTotal()));
        _server[i].initSocket();
        _serverMap.insert(
            std::pair<int, Server>(_server[i].getSocketFd(), _server[i]));
        fd = _server[i].getSocketFd();
        _listenFd.push_back(fd);
        _selectFd.push_back(fd);
        _maxFd = fd;
        ++i;
    }
}

Select::Select(Config& config)
{
    _activity = 0;
    _tv.tv_sec = 12;
    _tv.tv_usec = 0;

    initServer(config);
}

Select::~Select()
{
    for (selectIterator it = _selectFd.begin();
         it != _selectFd.end(); ++it)
        close(*it);
}
