#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>

template <class Multiplexer>
class WebServ;

enum e_context {MAIN_CONTEXT, HTTP_CONTEXT, SERVER_CONTEXT};
enum e_directive {
    LISTEN,
    SERVER_NAME,
    CLIENT_MAX_BODY_SIZE,
    ERROR_PAGE,
    RETURN,
    AUTOINDEX,
    INDEX,
    CGI
};

#define MAIN    1
#define HTTP    1 << 1
#define SERVER  1 << 2

typedef struct s_directive {
    uint8_t     mainMask;
    uint32_t    httpMask;
    uint32_t    serverMask;
}t_directive;

class Config {
public:
    void getMainContext(const std::string& context);
    void getHttpContext(const std::string& context);
    void getServerContext(const std::string& context);
    void getNextContext(const std::string& context);
    void getNextDirective(const std::string& line);
    void parseConfigFile(void);

    Config();
    Config(const std::string& configPath);
    ~Config();

private:
    std::string _configPath;

    typedef std::map<std::string,
            std::map<std::string,
            std::vector<std::string> > > context;

    context                     _mainContext;
    context                     _httpContext;
    context                     _serverContext;
    std::vector<t_directive>    _directive;
    std::vector<int>            _port;
};

/* NGINX context hierarchy
main
├── events
├── http
│   ├── server
│   │   ├── location
│   │   │   ├── if
│   │   │   └── limit_except
│   ├── upstream
│   ├── map
│   └── geo
├── stream
│   └── server
└── mail
    └── server
*/