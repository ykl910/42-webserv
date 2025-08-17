#pragma once

#include "textFormatting.hpp"
#include "Error.hpp"
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>

#define CONTEXT_NUMBER 1
#define GETTING_ALL_SERVERS 1
#define DIRECTIVE_NAME_LENGTH _configFormat[_contextIndex][_directiveIndex].length()

enum e_context_list {SERVER, ERROR, REDIRECTION, LOCATION, CGI};

enum e_directive {FORMAT};

enum e_server_directive {
    LISTEN,
    HOST,
    SERVER_NAME,
    CLIENT_MAX_BODY_SIZE
};

enum e_error_directive {
    E_400,
    E_401,
    E_402,
    E_403,
    E_404,
    // ...

    E_500,
    E_501,
    E_502
};

enum e_redirection_directive {
    R_300,
    R_301,
    R_302
};

enum e_location_directive {};

typedef struct s_cgi {

}t_cgi;

typedef struct s_location {

}t_location;

typedef struct s_redirection {

}t_redirection;

typedef struct s_error_page {
    std::string err_404;
    std::string err_500;
}t_error_page;

typedef struct s_server {
    int                         client_max_body_size;
    std::string                 port;
    std::string                 host;
    std::string                 server_name;
    t_error_page                error_page;
    t_redirection               redirection;
    std::vector<t_location>     location;
    std::vector<t_cgi>          cgi;
}t_server;

class Config {
public:
    typedef std::string directive;

    typedef uint32_t contextName;
    typedef uint32_t directiveName;

    typedef std::map<directiveName, directive> context;
    typedef context::const_iterator directiveIterator;

    typedef std::map<contextName, context> server;
    typedef server::const_iterator contextIterator;

    typedef std::map<contextName, context> configFormat;
    typedef configFormat::const_iterator configFormatIterator;

    typedef std::vector<server> configParser;
    typedef configParser::const_iterator configParserIterator;

    typedef std::vector<t_server> config;
    typedef config::const_iterator configIterator;

    void printConfig(void) const;
    void printConfigFormat(void) const;
    void printConfigParser(void) const;
    void printServer(const server& srv) const;

    bool isEndOfConfigFile(std::ifstream& file, std::string& line);

    bool contextFormatValid(const std::string& line);
    bool directiveFormatValid(const std::string& line, int indentSize);

    void getServer(std::ifstream& file, std::string& line, server& server);
    void getContext(std::ifstream& file, std::string& line, server& server);
    void getDirective(std::string& line, directive& newDirective, int indentSize);

    void storeConfig(void);
    void parseConfigFile(void);
    void initConfigParser(void);

    Config(const char* configFilePath);
    ~Config();

private:
    int             _serverMask;
    int             _contextIndex;
    int             _directiveIndex;
    configParser    _configParser;
    configFormat    _configFormat;
    config          _config;
    std::string     _configFilePath;
};
