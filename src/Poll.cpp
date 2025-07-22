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
    serverPoll.events = POLLIN | POLLERR;
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
    newClientPoll.events = POLLIN | POLLERR;
    newClientPoll.revents = 0;
    _pollFd.push_back(newClientPoll);
}

void    Poll::manageRequest(pollIterator& it, ssize_t bytes) {
    std::string request(_buffer, bytes);

    HttpRequest httpReq(request);
    std::cout << BOLD ITALIC GREEN <<  "Received request:\n" << DEFAULT;
    std::cout << MAGENTA << request << DEFAULT << std::endl;

    HttpResponse httpRes(httpReq);
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
    server.printServerStatus("poll", getConfigFilePath());

    initPoll();
    ssize_t bytes;
    while (true) {
        int activity = poll(&_pollFd[0], _pollFd.size(), 10); // 10 ms timeout
        if (activity == -1)
            printError();

        // if socket got a new client
        if (_pollFd[0].revents & POLLIN) {
            acceptClient(_pollFd[0].fd);
            std::cout << BOLD WHITE << "Poll got a new client\n" << DEFAULT;
        }

        for (pollIterator it = _pollFd.begin() + 1;
                          it != _pollFd.end();) {
            if (it->revents & POLLIN) {
                bytes = recv(it->fd, _buffer, sizeof(_buffer), 0);
                if (bytes <= 0) {
                    close(it->fd);
                    it = _pollFd.erase(it);
                    continue;
                } else
                    manageRequest(it, bytes);
            }
        }
    }
}

Poll::Poll(const char* configFilePath) : Socket(configFilePath) {}

Poll::~Poll() {
    for (pollIterator it = _pollFd.begin();
                      it != _pollFd.end(); it++) {
        close(it->fd);
    }
}
