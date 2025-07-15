#pragma once

#include "Socket.hpp"
#include "Error.hpp"
#include <sys/poll.h>
#include <sys/time.h>
#include <vector>

template <class Multiplexer>
class WebServ;

class Poll : public Socket {
public:
    void initPoll(void);
    void run(WebServ<Poll>& server);

    Poll();
    ~Poll();

private:
    typedef std::vector<struct pollfd>::iterator pollIterator;

    nfds_t nfds;
    char buffer[4092];
    std::vector<struct pollfd> _pollFd;
};
