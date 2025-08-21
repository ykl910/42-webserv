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
    int                         _client_max_body_size;
    std::string                 _server_name;
    t_error_page                _error_page;
    t_redirection               _redirection;
    std::vector<t_location>     _location;
    std::vector<t_cgi>          _cgi;
}t_serv_attr;

class Server {
public:
    int getSocketFd(void) const;
    void setServerAttr(void);
    std::string _port;
    std::string _host;
    Socket      _socket;

    Server(server& config);
    ~Server();

private:
    int                         _client_max_body_size;
    std::string                 _server_name;
    t_error_page                _error_page;
    t_redirection               _redirection;
    std::vector<t_location>     _location;
    std::vector<t_cgi>          _cgi;
    t_serv_attr                 _attribute;
};