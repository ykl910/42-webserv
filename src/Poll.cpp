#include "../include/Poll.hpp"
#include "../include/WebServ.hpp"
// int ppoll(struct pollfd *fds, nfds_t nfds,
//      const struct timespec *tmo_p, const sigset_t *sigmask);
// struct pollfd {
//        int   fd;         /* file descriptor */
//        short events;     /* requested events */
//        short revents;    /* returned events */
// };

void    Poll::run(WebServ<Poll>& server) {
    server.printServerStatus("poll");

    // struct timeval tv;
    // ppoll(this->_fds, 10, )
}

Poll::Poll() {

}

Poll::~Poll() {

}
