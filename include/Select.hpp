#pragma once

#include "Socket.hpp"
#include <sys/select.h>
#include <vector>

template <class Multiplexer>
class WebServ;

class Select : public Socket {
public:
    void initSelect(void);
    void checkReadFds(void);
    void checkWriteFds(void);
    void checkExceptFds(void);
    void run(WebServ<Select>& server);

    int acceptClient(void);

    Select();
    ~Select();

private:
    typedef std::vector<int>::iterator selectIterator;

    char                _buffer[4096];
    fd_set              _readFds;
    fd_set              _writeFds;
    fd_set              _exceptFds;
    std::vector<int>    _selectFd;
};

/*
  fd_set
       A structure type that can represent a set of file descriptors.   According  to  POSIX,
       the  maximum  number  of  file  descriptors in an fd_set structure is the value of the
       macro FD_SETSIZE.
*/