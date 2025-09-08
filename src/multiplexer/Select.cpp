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

void    Select::run()
{
    while (true) {
        FD_ZERO(&_readFds);
        /*
            This  macro  clears  (removes all file descriptors from) set.
            It should be employed as the first step in initializing a file
            descriptor set.
        */
       for (size_t i = 0; i < _selectFd.size(); ++i) {
            FD_SET(_selectFd[i], &_readFds);
            if (_selectFd[i] > _maxFd)
                _maxFd = _selectFd[i];
       }
        /*
            This macro adds the file descriptor fd to set. Adding a file
            descriptor that is already present in the set is a no-op, and does
            not produce an error.
        */

        //* wait for event in a socket
        errno = 0;
        _activity = select(_maxFd + 1, &_readFds, NULL, NULL, &_tv);
        if (g_signal == SIGINT)
            return;
        else if (_activity == -1) {
            printError();
        } else if (_activity == 0) {
            continue;
        }

        for (selectIterator it = _selectFd.begin(); it != _selectFd.end();) {
            if (FD_ISSET(*it, &_readFds)) {
                if (isSocketFd(*it)) {
                /*
                    select() modifies the contents of the sets according to the rules
                    described below. After  calling  select(),  the FD_ISSET() macro
                    can be used to test if a file descriptor is still present in a set.
                    FD_ISSET() returns nonzero  if  the file descriptor fd is present
                    in set, and zero if it is not.
                */
                    //* new connexion -> accept connexion and add client to the list
                    int clientFd = _serverMap[*it].getSocket().acceptClient();
                    if (clientFd) {
                        _clientFd.push_back(clientFd);
                        _clientMap.insert(
                            std::pair<int, int>(clientFd, _serverMap[*it].getSocketFd()));
                        std::cout
                        << BOLD WHITE << "Select: new client accepted with fd "
                        << BOLD BLUE << clientFd << DEFAULT << "\n";
                    }
                    ++it;
                } else { //* read request and send response
                    int serverFd = _clientMap[*it];
                    HttpManager(int(*it), _serverMap[serverFd].getServerAttribute(), _state);
                    if (HttpManager::hasCompletedResponse(*it)) {
                        close(*it);
                        _clientMap.erase(*it);
                        it = _selectFd.erase(it);
                    } else {
                        ++it;
                    }

                }
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

        _server.push_back(Server(serverConfig));
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
    _state = -1;
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
