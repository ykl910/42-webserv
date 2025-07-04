#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "textFormatting.hpp"
#include <poll.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <iostream>
#include <vector>

typedef struct s_config {
    const std::string *name;
}t_config;

class WebServ {
public:
    WebServ();
    WebServ(t_config& config);
    ~WebServ();

private:
    const std::vector<std::string>  server_name;
    struct pollfd   fds;
};

#endif