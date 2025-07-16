#include "../include/Select.hpp"
#include "../include/WebServ.hpp"

int Select::acceptClient(void) {
    return 0;
}

void    Select::initSelect(void) {
    return;
}

void    Select::checkReadFds() {
    return;
}

void    Select::checkWriteFds() {
    return;
}

void    Select::checkExceptFds() {
    return;
}

void    Select::run(WebServ<Select>& server) {
    server.printServerStatus("select");

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    int maxFd = this->getSocketFd();
    int serverFd = this->getSocketFd();
    int activity = 0;

    while (true) {

        FD_ZERO(&_readFds);
        /*
            This  macro  clears  (removes all file descriptors from) set.
            It should be employed as the first step in initializing a file
            descriptor set.
        */

        FD_SET(serverFd, &_readFds);
        /*
            This macro adds the file descriptor fd to set.  Adding a file
            descriptor  that is already present in the set is a no-op, and does
            not produce an error.
        */

        //* Add every client sockets to readFds and add maxFd if necessary
        for (selectIterator it = this->_selectFd.begin();
                            it != this->_selectFd.end(); ++it) {
            FD_SET(*it, &_readFds);
            if (*it > maxFd)
                maxFd = *it;
        }

        //* wait for event in a socket
        errno = 0;
        activity = select(maxFd + 1, &_readFds, &_writeFds, &_exceptFds, &tv);
        if (activity < 0) {
            printError(); // select error
            continue;
        } else if (activity == 0) {
            printError(); // timeout
        }

        //* new connexion -> accept connexion and add client to the list
        if (FD_ISSET(serverFd, &_readFds)) {
        /*
            select() modifies the contents of the sets according to the rules
            described below. After  calling  select(),  the FD_ISSET() macro
            can be used to test if a file descriptor is still present in a set.
            FD_ISSET() returns nonzero  if  the file descriptor fd is present
            in set, and zero if it is not.
        */
            errno = 0;
            int newClient = accept(serverFd, NULL, NULL);
            if (newClient < 0) {
                printError();
                continue;
            } else {
                this->_selectFd.push_back(newClient);
                std::cout << "New client connected: FD " << newClient << std::endl;
            }
        }

        //* loop on every actives clients and seek for data to read
        for (selectIterator it = this->_selectFd.begin();
                            it != this->_selectFd.end(); it++) {
            int bytes = 0;

            if (FD_ISSET(*it, &_readFds)) {
                bytes = recv(*it, this->_buffer, sizeof(_buffer), 0);
                if (bytes <= 0) {
                    close(*it);
                    std::cout << "Client disconnected: FD " << *it << std::endl;
                    it = this->_selectFd.erase(it);
                    continue;
                }

                std::string request(_buffer, bytes);
                HttpRequest httpReq(request);
                std::cout << "Received request:\n" << request << std::endl;
                std::cout << "Path:\n" << httpReq.getPath() << std::endl;
                HttpResponse httpRes(httpReq);
                httpRes.build(httpReq);
                std::string response = httpRes.getResponse();
                send(*it, response.c_str(), response.size(), 0);
                close(*it);
                it = this->_selectFd.erase(it);
            }
            else
                ++it;
        }
    }
}

Select::Select() {}

Select::~Select() {
    for (selectIterator it = this->_selectFd.begin();
         it != this->_selectFd.end(); ++it)
        close(*it);
}
