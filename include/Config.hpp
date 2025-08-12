#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>

template <class Multiplexer>
class WebServ;

enum e_context_list {SERVER, ERROR, REDIRECTION, LOCATION, CGI};

enum e_directive {FORMAT};

enum e_server_directive {
    SERVER_NAME,
    HOST,
    LISTEN,
    CLIENT_MAX_BODY_SIZE
};

enum e_error_directive {
    E_400,
    E_401,
    E_402,
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

typedef struct s_server {
    int                         client_max_body_size;
    std::string                 port;
    std::string                 domain;
    std::vector<std::string>    error_page;
    std::vector<std::string>    location;
    std::vector<std::string>    cgi;
}t_server;

class Config {
public:
    typedef uint32_t contextName;
    typedef uint32_t directiveName;

    typedef std::string directive;

    typedef std::map<directiveName, directive> context;
    typedef context::const_iterator contextIterator;

    typedef std::map<contextName, context> server;
    typedef server::const_iterator serverIterator;

    typedef std::map<contextName, context> configFormat;
    typedef configFormat::const_iterator configFormatIterator;

    typedef std::vector<server> config;
    typedef config::const_iterator configIterator;

    // typedef std::vector<t_server> config;
    // typedef config::const_iterator configIterator;

    void printConfigFormat(void) const;

    void printConfig(void) const;
    void printServer(const server& srv) const;

    bool isEndOfConfigFile(std::ifstream& file, std::string& line);
    bool directiveFormatValid(const std::string& line);
    bool contextFormatValid(std::string& line);
    bool contextDirectiveFormatValid(std::string& line, int indentSize);

    void getContextDirective(std::string& line, directive& newDirective, int indentSize);
    void getContext(std::ifstream& file, std::string& line, server& server);
    void getServer(std::ifstream& file, std::string& line, server& server);

    void parseConfigFile(void);
    void initConfigParser(void);
    const char* getConfigFilePath(void) const;

    Config(const char* configFilePath);
    ~Config();

private:
    std::string     _configFilePath;
    config          _config;
    configFormat    _configFormat;
    int             _contextIndex;
    int             _directiveIndex;
    int             _serverMask;
};
