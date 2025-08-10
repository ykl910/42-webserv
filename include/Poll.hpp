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

    void initPoll(void);
    void addClientToPoll(int clientFd);
    void run(WebServ<Poll>& server);

    Poll(const char* configFilePath);
    ~Poll();

private:
    int                         _activity;
    std::vector<struct pollfd>  _pollFd;
};
