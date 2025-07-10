#pragma once

#include "Socket.hpp"
#include <sys/poll.h>

template <class Multiplexer>
class WebServ;

class Poll : public Socket {
public:
    void run(WebServ<Poll>& server);

    Poll();
    ~Poll();

private:
    struct pollfd _fd;

};