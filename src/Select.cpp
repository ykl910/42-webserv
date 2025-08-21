#include "../include/Select.hpp"

bool    Select::isSocketFd(int fd) const
{
    for (size_t i = 0; i < _listenFd.size(); ++i) {
        if (fd == _listenFd[i])
            return true;
    }
    return false;
}

std::vector<Server*> Select::getServer(void) const {
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
       for (serverIterator it = _server.begin(); it != _server.end(); ++it) {
            int fd = (*it)->getSocketFd();
            FD_SET(fd, &_readFds);
            std::cout << fd << std::endl;
            if (fd > _maxFd)
                _maxFd = fd;
       }
        /*
            This macro adds the file descriptor fd to set. Adding a file
            descriptor that is already present in the set is a no-op, and does
            not produce an error.
        */
        for (selectIterator it = _clientFd.begin();
                            it != _clientFd.end(); ++it) {
            FD_SET(*it, &_readFds);
            if (*it > _maxFd)
                _maxFd = *it;
        }
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
        if (FD_ISSET(_listenFd[0], &_readFds)) {
        /*
            select() modifies the contents of the sets according to the rules
            described below. After  calling  select(),  the FD_ISSET() macro
            can be used to test if a file descriptor is still present in a set.
            FD_ISSET() returns nonzero  if  the file descriptor fd is present
            in set, and zero if it is not.
        */
            //* new connexion -> accept connexion and add client to the list
            int clientFd = _server[0]->_socket.acceptClient();
            if (clientFd)
                _clientFd.push_back(clientFd);
            std::cout << BOLD WHITE << "Select: new client accepted with fd "
            << BOLD BLUE << clientFd << DEFAULT << "\n";
        }

        for (selectIterator it = _clientFd.begin();
                            it != _clientFd.end();) {
            //* read request and send response
            if (FD_ISSET(*it, &_readFds)) {
                HttpManager(int(*it));
                close(*it);
                it = _clientFd.erase(it);
            } else
                ++it;
        }
    }
}

void    Select::createServer(Config& config)
{
    configParser parser = config.getConfigParser();
    for (configParserIterator it = parser.begin();
                              it != parser.end(); ++it) {
        server serverConfig = *it;
        _server.push_back(new Server(serverConfig));
    }
}

Select::Select(Config& config)
{
    _activity = 0;
    _tv.tv_sec = 10;
    _tv.tv_usec = 0;

    createServer(config);
    for (serverIterator it = _server.begin(); it != _server.end(); ++it)
        _listenFd.push_back((*it)->getSocketFd());
    _maxFd = _listenFd[0];
}

Select::~Select()
{
    std::cout << "select destructor called\n";
    // if (_socketFd)
    //     close(_socketFd);
    for (selectIterator it = _clientFd.begin();
         it != _clientFd.end(); ++it)
        close(*it);
}
