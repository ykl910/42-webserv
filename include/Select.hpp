#pragma once

#include "Socket.hpp"
#include <sys/select.h>
#include <vector>

template <class Multiplexer>
class WebServ;

class Select : public Socket {
public:
    bool acceptClient(int serverFd);
    void readAndWrite(void);
    void run(WebServ<Select>& server);

    Select();
    ~Select();

private:
    typedef std::vector<int>::iterator selectIterator;
    fd_set              _readFds;
    std::vector<int>    _selectFd;
};

/*
  fd_set
       A structure type that can represent a set of file descriptors.
       According  to  POSIX, the  maximum  number  of  file  descriptors in an
       fd_set structure is the value of the macro FD_SETSIZE.
*/
