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
    void createServer(Config& config);
    void printServerStatus(const char* multiplexer, const char* configFilePath);

    WebServ(Config& config, const char* multiplexer);
    ~WebServ();

private:
    std::vector<Server> _server;
    Multiplexer         _multiplexer;
};
#include "../src/WebServ.tpp"
