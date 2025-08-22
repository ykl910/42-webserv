#pragma once

#include "Config.hpp"
#include "Socket.hpp"
#include <iostream>

typedef struct s_cgi {

}t_cgi;

typedef struct s_location {

}t_location;

typedef struct s_redirection {
    std::string redir_300;
    std::string redir_301;
}t_redirection;

typedef struct s_error_page {
    std::string err_404;
    std::string err_500;
}t_error_page;

typedef struct s_serv_attr {
    int                         client_max_body_size;
    std::string                 port;
    std::string                 host;
    std::string                 listen;
    std::string                 server_name;
    t_error_page                error_page;
    t_redirection               redirection;
    std::vector<t_location>     location;
    std::vector<t_cgi>          cgi;
}t_serv_attr;

class Server {
public:
    Socket& getSocket(void);
    int getSocketFd(void) const;
    t_serv_attr& getServerAttribute(void);

    Server(server& config);
    ~Server();

private:
    t_serv_attr _attribute;
    Socket      _socket;
};