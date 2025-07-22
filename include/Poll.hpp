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
    typedef std::vector<struct pollfd>::iterator pollIterator;

    void acceptClient(int socketFd);
    void manageRequest(pollIterator& it, ssize_t bytes);
    void initPoll(void);
    void run(WebServ<Poll>& server);

    Poll(const char* configFilePath);
    ~Poll();

private:
    //nfds_t nfds;
    char _buffer[4092];
    std::vector<struct pollfd> _pollFd;
};
