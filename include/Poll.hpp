#pragma once

#include "Error.hpp"
#include <sys/poll.h>

class WebServ;

class Poll : public Error {
public:
    void run(WebServ& server);

    Poll();
    ~Poll();

private:

};