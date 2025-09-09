#pragma once

// #include "HttpManager.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include <iostream>

typedef struct s_cgi {

}t_cgi;

typedef struct s_location {
    std::string path;
    std::string root;
    std::string index;
    bool        autoindex;
    uint8_t     method;
}t_location;

typedef struct s_redirection {
    std::string redir_300;
    std::string redir_301;
}t_redirection;

typedef struct s_error_page {
    std::string err_400;
    std::string err_403;
    std::string err_404;
    std::string err_500;
}t_error_page;

typedef struct s_serv_attr {
    std::string         host;
    std::string         port;
    std::string         server_name;
    int                 client_max_body_size;
    bool                autoindex;
    t_location          location;
    t_error_page        error_page;
    t_redirection       redirection;
    std::vector<t_cgi>  cgi;
}t_serv_attr;

class HttpManager;

class Server {
public:
    void initSocket(void);

    void storeCgi(server& config);
    void storeLocation(server& config);
    void storeErrorPage(server& config);
    void storeRedirection(server& config);

    Socket& getSocket(void);
    int getSocketFd(void) const;
    t_serv_attr& getServerAttribute(void);
    int getClientMaxBodySize(const std::string& input);

    Server() {}
    Server(server& config);
    Server& operator=(Server& other);
    ~Server();

private:
    t_serv_attr         _attribute;
    Socket              _socket;
};