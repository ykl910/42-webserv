#include "../include/Select.hpp"
#include "../include/HttpManager.hpp"
#include "../include/WebServ.hpp"

void    Select::run()
{
    while (true) {
        FD_ZERO(&_readFds);
        /*
            This  macro  clears  (removes all file descriptors from) set.
            It should be employed as the first step in initializing a file
            descriptor set.
        */
        FD_SET(_socketFd, &_readFds);
        /*
            This macro adds the file descriptor fd to set. Adding a file
            descriptor that is already present in the set is a no-op, and does
            not produce an error.
        */
        for (selectIterator it = _selectFd.begin();
                            it != _selectFd.end(); ++it) {
            FD_SET(*it, &_readFds);
            if (*it > _maxFd)
                _maxFd = *it;
        }
        //* wait for event in a socket
        errno = 0;
        _activity = select(_maxFd + 1, &_readFds, NULL, NULL, &_tv);
        if (_activity == -1) {
            printError();
        } else if (_activity == 0) {
            continue;
        }
        if (FD_ISSET(_socketFd, &_readFds)) {
        /*
            select() modifies the contents of the sets according to the rules
            described below. After  calling  select(),  the FD_ISSET() macro
            can be used to test if a file descriptor is still present in a set.
            FD_ISSET() returns nonzero  if  the file descriptor fd is present
            in set, and zero if it is not.
        */
            //* new connexion -> accept connexion and add client to the list
            int clientFd = _socket.acceptClient();
            if (clientFd)
                _selectFd.push_back(clientFd);
            std::cout << BOLD WHITE << "Select: new client accepted with fd "
            << BOLD BLUE << clientFd << DEFAULT << "\n";
        }

        for (selectIterator it = _selectFd.begin();
                            it != _selectFd.end();) {
            //* read request and send response
            if (FD_ISSET(*it, &_readFds)) {
                HttpManager(int(*it));
                close(*it);
                it = _selectFd.erase(it);
            } else
                ++it;
        }
    }
}

Select::Select() : _socket()
{
    _activity = 0;
    _tv.tv_sec = 10;
    _tv.tv_usec = 0;
    _socketFd = _socket.getSocketFd();
    _maxFd = _socketFd;
}

Select::~Select()
{
    for (selectIterator it = _selectFd.begin();
         it != _selectFd.end(); ++it)
        close(*it);
}
