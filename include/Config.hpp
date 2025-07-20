#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>

template <class Multiplexer>
class WebServ;

enum e_token    {SEMI_COLON, OPENING_CURLY_BRACE, CLOSING_CURLY_BRACE};
enum e_context  {MAIN_CONTEXT, HTTP_CONTEXT, SERVER_CONTEXT, LOCATION_CONTEXT};

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

typedef struct s_directive {
    std::string                 name;
    std::vector<std::string>    argument;
}t_directive;

typedef struct s_context {
    std::vector<t_directive>        directive;
    std::vector<struct s_context>   context;
}t_context;

typedef struct s_config_node {

}t_config_node;

class Config {
public:
    typedef std::vector<t_config_node>  configParser;

    bool isDirective(const std::string& line);
    void getMainContext(std::ifstream& file, std::string& line);
    void getHttpContext(std::ifstream& file, std::string& line);
    void getServerContext(std::ifstream& file, std::string& line);

    void parseConfigFile(void);
    void printConfig(void);

    Config();
    Config(const std::string& configPath);
    ~Config();

private:
    std::string                 _configPath;
    t_configMask                _configMask;
    configParser                _configParser;
    std::vector<std::string>    _port;

};
