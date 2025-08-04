#pragma once

#include "Socket.hpp"
#include "utils.hpp"
#include <sys/select.h>
#include <vector>

template <class Multiplexer>
class WebServ;

class Select : public Socket {
public:
    // HttpRequest readCompleteRequest(std::string *requestData, int fd, bool &success);
    void initSelect(void);
    void run(WebServ<Select>& server);

    Select(const char *configFilePath);
    ~Select();

private:
    typedef std::vector<int>::iterator selectIterator;

    int                 _maxFd;
    int                 _activity;
    int                 _socketFd;
    fd_set              _readFds;
    struct timeval      _tv;
    std::vector<int>    _selectFd;
};

/*
  fd_set
       A structure type that can represent a set of file descriptors.
       According  to  POSIX, the  maximum  number  of  file  descriptors in an
       fd_set structure is the value of the macro FD_SETSIZE.
*/
