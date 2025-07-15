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

    serverPoll.fd = this->getServerFd();
    this->_pollFd.push_back(serverPoll);
}

void    Poll::run(WebServ<Poll>& server) {
    server.printServerStatus("poll");

    this->initPoll();
    int bytes;
    while (true) {
        int activity = poll(&this->_pollFd[0], this->_pollFd.size(), 5000); // 5 ms timeout
        if (activity < 0) {
            printError(); //
            continue;
        } else if (activity == 0) {
            printError(); // timeout
        }
        for (pollIterator it = this->_pollFd.begin();
                          it != this->_pollFd.end(); it++) {

            if (it->fd)  {
                switch (it->events) {
                    case POLLIN: // There is data to read
                    bytes = recv(it->fd, this->buffer, sizeof(this->buffer), 0);
                    break;

                    case POLLPRI:
                    break;

                    case POLLOUT:
                    break;

                    case POLLRDHUP:
                    break;

                    case POLLERR:
                    break;

                    case POLLHUP:
                    break;

                    case POLLNVAL:
                    break;

                    case POLLRDNORM:
                    break;

                    case POLLRDBAND:
                    break;

                    case POLLWRNORM:
                    break;

                    case POLLWRBAND:
                    break;
                }
                switch (it->revents) {

                }
            }
        }
    }
}

Poll::Poll() {

}

Poll::~Poll() {
    for (pollIterator it = this->_pollFd.begin();
                      it != this->_pollFd.end(); it++) {
    }
}
