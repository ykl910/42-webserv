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
    int maxFd = this->getServerFd();
    int serverFd = this->getServerFd();
    int activity = 0;

    while (true) {

        //* FD_ZERO = empty readFds set
        //* FD_SET = add server socket to detect new connexions
        FD_ZERO(&_readFds);
        FD_SET(serverFd, &_readFds);

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
            char buffer[4096];
            int bytes = 0;

            if (FD_ISSET(*it, &_readFds)) {
                bytes = recv(*it, buffer, sizeof(buffer), 0);
                if (bytes <= 0) {
                    close(*it);
                    std::cout << "Client disconnected: FD " << *it << std::endl;
                    it = this->_selectFd.erase(it);
                    continue;
                }

                std::string request(buffer, bytes);
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
