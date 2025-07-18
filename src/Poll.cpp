#include "../include/Poll.hpp"
#include "../include/WebServ.hpp"

// int ppoll(struct pollfd *fds, nfds_t nfds,
//      const struct timespec *tmo_p, const sigset_t *sigmask);
// struct pollfd {
//        int   fd;         /* file descriptor */
//        short events;     /* requested events */
//        short revents;    /* returned events */
// };

void    Poll::initPoll(void) {
    struct pollfd serverPoll;

    serverPoll.fd = getSocketFd();
    std::cout << serverPoll.fd << std::endl;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;
    _pollFd.push_back(serverPoll);
}

void    Poll::acceptClient(int socketFd) {
    int clientFd = accept(socketFd, NULL, NULL);
    if (clientFd == -1)
        printErrorAndThrow("accept");

    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
        printErrorAndThrow("fcntl");

    struct pollfd newClientPoll;

    newClientPoll.fd = clientFd;
    newClientPoll.events = POLLIN;
    newClientPoll.revents = 0;
    _pollFd.push_back(newClientPoll);
    std::cout << "Here\n";
}

void    Poll::manageRequest(pollIterator& it, ssize_t bytes) {
    std::string request(_buffer, bytes);

    HttpRequest httpReq(request);
    std::cout << "Received request:\n" << request << std::endl;

    HttpResponse httpRes(httpReq);
    httpRes.build(httpReq);
    std::string response = httpRes.getResponse();
    ssize_t totalSent = 0;

    const char* data = response.c_str();
    ssize_t totalSize = response.size();
    while (totalSent < totalSize) {
        ssize_t sent = send(it->fd, data + totalSent, totalSize - totalSent, 0);
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
    close(it->fd);
    it = _pollFd.erase(it);
}

void    Poll::run(WebServ<Poll>& server) {
    server.printServerStatus("poll");

    // int socketFd = getSocketFd();
    initPoll();
    ssize_t bytes;
    while (true) {
        std::cout << "size: " << _pollFd.size() << std::endl;
        int activity = poll(&_pollFd[0], _pollFd.size(), 10); // -1 wait indefinitely
        if (activity == -1)
            printError();

        size_t i = 0;
        // std::cout << "Here\n";
        for (pollIterator it = _pollFd.begin();
                          it != _pollFd.end();) {
            if (it->revents & POLLIN) {
                std::cout << i << std::endl;
            } else {
               bytes = recv(it->fd, _buffer, sizeof(_buffer), 0);
               if (bytes <= 0) {
                    close(it->fd);
                    it = _pollFd.erase(it);
               } else
                    manageRequest(it, bytes);
            }
            i++;
        }
        // acceptClient(socketFd);
        std::cout << _pollFd[0].fd << std::endl;
        acceptClient(_pollFd[0].fd);
    }
}

Poll::Poll() {

}

Poll::~Poll() {
    for (pollIterator it = _pollFd.begin();
                      it != _pollFd.end(); it++) {
    }
}
