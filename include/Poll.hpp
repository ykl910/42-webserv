#pragma once

#include <sys/poll.h>

class WebServ;

class Poll {
public:
    void run(WebServ& server);

    Poll();
    ~Poll();

private:

};