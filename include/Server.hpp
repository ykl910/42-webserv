#pragma once

// #include "HttpManager.hpp"
#include "Config.hpp"
#include "Socket.hpp"
#include <algorithm>
#include <iostream>

typedef struct s_cgi {
    std::string extension;
    std::vector<std::string> path;
}t_cgi;

typedef struct s_location {
    std::string path;
    std::string root;
    std::string index;
    bool        autoindex;
    uint8_t     method;
}t_location;

typedef struct s_redirection {
    std::vector<std::string> redir_301;
    std::vector<std::string> redir_302;
}t_redirection;

typedef struct s_error_page {
    std::string err_400;
    std::string err_403;
    std::string err_404;
    std::string err_405;
    std::string err_500;
}t_error_page;

typedef std::map<std::string, std::vector<std::string> > cgiMap;

typedef struct s_serv_attr {
    std::string             host;
    std::string             port;
    std::string             server_name;
    int                     client_max_body_size;
    bool                    autoindex;
    std::vector<t_location> location;
    t_error_page            error_page;
    t_redirection           redirection;
    // std::vector<t_cgi>      cgi;
    cgiMap                  cgi;
}t_serv_attr;

class HttpManager;

class Server {
public:
    void initSocket(void);

    void storeLocation(server& config, size_t locationNbr);
    void storeErrorPage(server& config, size_t locationNbr);
    void storeRedirection(server& config, size_t locationNbr);
    void storeCgi(server& config, size_t locationNbr, size_t cgiNbr, size_t cgiTotal);

    bool methodAlreadyDefined(uint8_t mask, size_t method);
    void getRedirectionValue(server& config, size_t locationNbr, std::vector<std::string>& redir, int i);

    Socket& getSocket(void);
    int getSocketFd(void) const;
    t_serv_attr& getServerAttribute(void);
    int getClientMaxBodySize(const std::string& input);

    Server() {}
    Server(server& config, size_t locationNbr, size_t cgiNbr, size_t cgiTotal);
    Server& operator=(Server& other);
    ~Server();

private:
    t_serv_attr _attribute;
    Socket      _socket;
};