#pragma once

#include "textFormatting.hpp"
#include "Select.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include "Error.hpp"
#include "utils.hpp"
#include "Epoll.hpp"
#include "Poll.hpp"
#include "CGI.hpp"

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

typedef struct s_socket {
    int             fd;
    struct addrinfo hints;
}t_socket;

typedef struct s_server {
    int                         client_max_body_size;
    std::string                 port;
    std::string                 host;
    std::string                 server_name;
    t_error_page                error_page;
    t_redirection               redirection;
    std::vector<t_location>     location;
    std::vector<t_cgi>          cgi;
    Socket                      socket;
}t_server;

typedef std::vector<t_server> config;
typedef config::iterator configIterator;

template <class Multiplexer>
class WebServ {
public:
    void createServer(void);
    void printServerStatus(const char* multiplexer, const char* configFilePath);

    WebServ(const char* configFilePath, const char* multiplexer);
    ~WebServ();

private:
    Config      _config;
    Multiplexer _multiplexer;
    config      _server;
};
#include "../src/WebServ.tpp"
