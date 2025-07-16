#pragma once

#include "Socket.hpp"
#include <sys/select.h>
#include <vector>

template <class Multiplexer>
class WebServ;

class Select : public Socket {
public:
    void run(WebServ<Select>& server);

    Select();
    ~Select();

private:
    typedef std::vector<int>::iterator fdsIterator;
    std::vector<int> _clientFds;

};
