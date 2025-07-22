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

class Config {
public:
    typedef uint32_t contextName;
    typedef uint32_t directiveName;

    typedef std::vector<std::string> directive;
    typedef std::map<directiveName, directive> context;
    typedef std::map<contextName, context> server;
    typedef std::vector<server> webservConfig;

    typedef std::map<contextName, context> contextList;
    typedef contextList configFormat;

    typedef contextList::const_iterator contextListIterator;
    typedef context::const_iterator contextIterator;

    void printConfigSyntax(void) const;
    void printServerConfig(void) const;

    void checkDirectiveFormat(const std::string& line);
    void checkContextFormat(const std::string& line);
    void getContextDirective(std::string& line, directive& newDirective);
    void getContext(std::ifstream& file, std::string& line, server& server);
    void getServer(std::ifstream& file, std::string& line, server& server);

    void parseConfigFile(void);
    void initConfigParser(void);
    const char* getConfigFilePath(void) const;

    Config(const char* configFilePath);
    ~Config();

private:
    std::string     _configFilePath;
    webservConfig   _webservConfig;
    configFormat    _configFormat;
    contextList     _contextList;
    int             _contextIndex;
    int             _directiveIndex;
    int             _serverMask;
    int             _directiveMask;
};
