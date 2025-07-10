#pragma once

#include "Socket.hpp"
#include <sys/poll.h>

class WebServ;

class Poll : public Error {
public:
    void run(WebServ& server);

    Poll();
    ~Poll();

private:

};