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

    serverPoll.fd = this->getSocketFd();
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
            if (it->events & POLLIN || it->events & POLLRDNORM) // Equivalent to POLLIN
            /*
                There is data to read.
            */
                bytes = recv(it->fd, this->buffer, sizeof(this->buffer), 0);

            else if (it->events & POLLPRI)
            /*
              There is some exceptional condition on the file descriptor.  Possibilities  in‐
              clude:

              •  There is out-of-band data on a TCP socket (see tcp(7)).

              •  A  pseudoterminal master in packet mode has seen a state change on the slave
                 (see ioctl_tty(2)).

              •  A cgroup.events file has been modified (see cgroups(7)).
            */
                return;

            else if (it->events & POLLOUT || it->events & POLLWRNORM) // Equivalent to POLLOUT
            /*
              Writing is now possible, though a write larger than the available  space  in  a
              socket or pipe will still block (unless O_NONBLOCK is set).
            */
                return;

            else if (it->events & POLLRDHUP)
            /*
              (since Linux 2.6.17) Stream  socket peer closed connection, or shut down writing half of connection.
              The _GNU_SOURCE feature test macro must be defined (before including any header
              files) in order to obtain this definition.
            */
                return;

            else if (it->events & POLLERR)
            /*
              Error condition (only returned in revents; ignored in  events). This  bit  is
              also  set  for  a file descriptor referring to the write end of a pipe when the
              read end has been closed.
            */
                return;

            else if (it->events & POLLHUP)
            /*
              Hang up (only returned in revents; ignored in events).  Note that when  reading
              from  a  channel such as a pipe or a stream socket, this event merely indicates
              that the peer closed its end of the channel.  Subsequent reads from the channel
              will return 0 (end of file) only after all outstanding data in the channel  has
              been consumed.
            */
                return;

            else if (it->events & POLLNVAL)
            /*
              Invalid request: fd not open (only returned in revents; ignored in events)
            */
                return;

            else if (it->events & POLLRDBAND)
            /*
              Priority band data can be read (generally unused on Linux)
            */
                return;

            else if (it->events & POLLWRBAND)
            /*
              Priority data may be written
            */
                return;
        }
        (void)bytes;
    }
}

Poll::Poll() {

}

Poll::~Poll() {
    for (pollIterator it = this->_pollFd.begin();
                      it != this->_pollFd.end(); it++) {
    }
}
