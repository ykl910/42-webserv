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
    typedef std::vector<server> config;
    typedef config::const_iterator configIterator;

    typedef std::map<contextName, context> configFormat;

    typedef configFormat::const_iterator contextFormatIterator;
    typedef context::const_iterator contextIterator;

    void printConfigFormat(void) const;
    void printConfig(void) const;

    bool isEndOfConfigFile(std::ifstream& file, std::string& line);
    bool isContextFormatValid(std::string& line);
    bool isContextDirectiveFormatValid(std::string& line, int indentSize);

    void checkDirectiveFormat(const std::string& line);
    void checkContextFormat(const std::string& line);
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
    config          _webservConfig;
    configFormat    _contextFormat;
    int             _contextIndex;
    int             _directiveIndex;
    int             _serverMask;
};
