#pragma once

#include "Socket.hpp"
#include <sys/select.h>
#include <vector>
#include "HttpRequest.hpp"

template <class Multiplexer>
class WebServ;

class Select : public Socket {
public:
    bool acceptClient(int serverFd);
    HttpRequest readCompleteRequest(std::string *requestData, int fd, bool &success);
    void manageRequest(void);
    void run(WebServ<Select>& server);

    Select(const char *configFilePath);
    ~Select();

private:
    typedef std::vector<int>::iterator selectIterator;
    fd_set              _readFds;
    std::vector<int>    _selectFd;
    struct timeval      _tv;
};

/*
  fd_set
       A structure type that can represent a set of file descriptors.
       According  to  POSIX, the  maximum  number  of  file  descriptors in an
       fd_set structure is the value of the macro FD_SETSIZE.
*/
