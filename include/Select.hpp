#pragma once

#include "Socket.hpp"
#include <sys/select.h>
#include <vector>

template <class Multiplexer>
class WebServ;

class Select : public Socket {
public:
    void initSelect(void);
    void run(WebServ<Select>& server);

    int acceptClient(void);

    Select();
    ~Select();

private:
    typedef std::vector<int>::iterator selectIterator;
    std::vector<int> _selectFd;
    fd_set _readFds;
};