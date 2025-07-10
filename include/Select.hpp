#pragma once

#include "Socket.hpp"
#include <sys/select.h>
#include <vector>

class WebServ;

class Select : public Socket {
public:
    void run(WebServ& server);

    Select();
    ~Select();

private:
    typedef std::vector<int>::iterator fdsIterator;
    std::vector<int> _clientFds;

};