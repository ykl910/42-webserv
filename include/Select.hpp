#pragma once

#include "HttpManager.hpp"
#include "WebServ.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "Signal.hpp"
#include <sys/select.h>
#include <vector>

typedef struct s_server         t_server;
typedef std::vector<t_server>   config;
typedef config::iterator        configIterator;

template <class Multiplexer>
class WebServ;

class Select {
public:
    void run();

    Select() {}
    Select(config& server);
    ~Select();

private:
    typedef std::vector<int>::iterator selectIterator;

    int                 _maxFd;
    int                 _activity;
    int                 _socketFd;
    fd_set              _readFds;
    struct timeval      _tv;
    std::vector<int>    _selectFd;
    config              _server;
};

/*
  fd_set
       A structure type that can represent a set of file descriptors.
       According  to  POSIX, the  maximum  number  of  file  descriptors in an
       fd_set structure is the value of the macro FD_SETSIZE.
*/
