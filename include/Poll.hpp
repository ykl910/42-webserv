#pragma once

#include "Socket.hpp"
#include <sys/poll.h>

template <class Multiplexer>
class WebServ;

class Poll : public AError {
public:
    void run(WebServ<Poll>& server);

    Poll();
    ~Poll();

private:

};