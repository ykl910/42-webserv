#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>

template <class Multiplexer>
class WebServ;

enum e_context {MAIN_CONTEXT, HTTP_CONTEXT, SERVER_CONTEXT, LOCATION_CONTEXT};

enum e_main_directive {

};

enum e_http_directive {
    CLIENT_MAX_BODY_SIZE_HTTP
};

enum e_server_directive {
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

typedef struct s_configMask {
    uint8_t     mainContext;
    uint32_t    httpContext;
    uint32_t    serverContext;
}t_configMask;

class Config {
public:
    typedef std::string context_name;
    typedef std::string subcontext_name;
    typedef std::string directive_name;

    typedef std::vector<std::string> directive_argument;
    typedef std::map<directive_name, directive_argument> context;
    typedef std::map<directive_name, directive_argument> directive;
    typedef std::map<subcontext_name, context> mainContext;
    typedef std::map<subcontext_name, context> httpContext;
    typedef std::map<subcontext_name, context> serverContext;

    typedef mainContext::iterator mainContextIt;
    typedef httpContext::iterator httpContextIt;
    typedef serverContext::iterator serverContextIt;
    typedef directive::iterator directiveIt;

    bool isDirective(const std::string& line);
    bool isDirective(const directiveIt& it);
    void getMainContext(std::ifstream& file, std::string& line);
    void getHttpContext(std::ifstream& file, std::string& line);
    void getServerContext(std::ifstream& file, std::string& line);
    void getNextContext(std::ifstream& file, context& context, std::string& line);

    template <typename Context>
    void getNextDirective(std::ifstream& file, Context& context,
                          std::string& line);
    void parseConfigFile(void);
    void printConfig(void);

    Config();
    Config(const std::string& configPath);
    ~Config();

private:
    std::string                 _configPath;
    t_configMask                _configMask;
    mainContext                 _mainContext;
    httpContext                 _httpContext;
    serverContext               _serverContext;
    std::vector<std::string>    _port;
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