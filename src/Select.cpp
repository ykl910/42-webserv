#include "../include/Select.hpp"
#include "../include/WebServ.hpp"

void    Select::acceptClient(int serverFd) {
    errno = 0;
    int newClient = accept(serverFd, NULL, NULL);
    if (newClient == -1) {
        printError();
    } else {
        int flags = fcntl(newClient, F_GETFL, 0);
        if (flags == -1 || fcntl(newClient, F_SETFL, flags | O_NONBLOCK) == -1) {
            printError();
            close(newClient);
        } else {
            _selectFd.push_back(newClient);
            std::cout << "New client connected: FD " << newClient << std::endl;
        }
    }
}

void    Select::manageRequest(void) {
    char buf[4096];
    ssize_t bytes = 0;

    for (selectIterator it = _selectFd.begin();
                        it != _selectFd.end();) {
        if (FD_ISSET(*it, &_readFds)) {
            bytes = recv(*it, buf, sizeof(buf), 0);
            if (bytes <= 0) {
                close(*it);
                std::cout << "Client disconnected: FD " << *it << std::endl;
                it = _selectFd.erase(it);
                continue;
            }
            std::string request(buf, bytes);
            HttpRequest httpReq(request);
            std::cout << BOLD ITALIC GREEN <<  "Received request:\n" << DEFAULT;
            std::cout << MAGENTA << request << DEFAULT << std::endl;
            HttpResponse httpRes(httpReq);
            std::string response = httpRes.getResponse();
            // std::cout << BOLD ITALIC GREEN <<  "Response sent:\n" << DEFAULT;
            // std::cout << YELLOW << response << DEFAULT << std::endl;
            ssize_t totalSent = 0;
            const char* data = response.c_str();
            ssize_t totalSize = response.size();
            while (totalSent < totalSize) {
                ssize_t sent = send(*it, data + totalSent, totalSize - totalSent, 0);
                if (sent < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    } else {
                        printError();
                        break;
                    }
                }
                totalSent += sent;
            }
            close(*it);
            it = _selectFd.erase(it);
        }
        else
            ++it;
    }
}

void    Select::run(WebServ<Select>& server) {
    server.printServerStatus("select");

    _tv.tv_sec = 10;
    _tv.tv_usec = 0;
    int maxFd = getSocketFd();
    int serverFd = getSocketFd();
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
            This macro adds the file descriptor fd to set. Adding a file
            descriptor that is already present in the set is a no-op, and does
            not produce an error.
        */
        for (selectIterator it = _selectFd.begin();
                            it != _selectFd.end(); ++it) {
            FD_SET(*it, &_readFds);
            if (*it > maxFd)
                maxFd = *it;
        }
        //* wait for event in a socket
        errno = 0;
        activity = select(maxFd + 1, &_readFds, NULL, NULL, &_tv);
        if (activity < 0) {
            printError();
            continue;
        } else if (activity == 0) {
            continue;
        }

        if (FD_ISSET(serverFd, &_readFds)) {
        /*
            select() modifies the contents of the sets according to the rules
            described below. After  calling  select(),  the FD_ISSET() macro
            can be used to test if a file descriptor is still present in a set.
            FD_ISSET() returns nonzero  if  the file descriptor fd is present
            in set, and zero if it is not.
        */
            //* new connexion -> accept connexion and add client to the list
            acceptClient(serverFd);
        }
        //* read request and send response
        manageRequest();
    }
}

Select::Select() {}

Select::~Select() {
    for (selectIterator it = _selectFd.begin();
         it != _selectFd.end(); ++it)
        close(*it);
}
