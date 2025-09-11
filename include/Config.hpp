#pragma once

#include "textFormatting.hpp"
#include "Error.hpp"
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>

#define NAME 0
#define VALUE 1
#define CONTEXT_NUMBER 1
#define GETTING_ALL_SERVERS 1
#define DIRECTIVE_NAME_LENGTH _configFormat[_contextIndex][_directiveIndex][NAME].length()

enum e_context_list {
    SERVER,
    LOCATION,
    ERROR,
    REDIRECTION,
    CGI
};

enum e_server_directive {
    LISTEN,
    HOST,
    SERVER_NAME,
    CLIENT_MAX_BODY_SIZE
};

enum e_location_directive {
    PATH,
    ROOT,
    INDEX,
    AUTOINDEX,
    METHOD
};

enum e_error_directive {
    E_400,
   // E_401,
   // E_402,
    E_403,
    E_404,
    // ...

    E_500,
    E_501
   // E_502
};

enum e_redirection_directive {
    R_301,
    R_302
};

typedef std::vector<std::string> directiveValue;
typedef std::vector<std::string>::const_iterator directiveValueIterator;

typedef uint32_t contextName;
typedef uint32_t directiveName;

typedef std::map<directiveName, directiveValue> context;
typedef context::const_iterator directiveIterator;

typedef std::map<contextName, context> server;
typedef server::const_iterator contextIterator;

typedef std::map<contextName, context> configFormat;
typedef configFormat::const_iterator configFormatIterator;

typedef std::vector<server> configParser;
typedef configParser::const_iterator configParserIterator;

class Config {
public:
    void printConfig(void) const;
    void printConfigFormat(void) const;
    void printServer(const server& srv) const;

    size_t getCgiTotal(void);
    size_t getCgiNbr(size_t i);
    size_t getLocationNbr(size_t i);
    configParser& getConfigParser(void);
    const char*& getConfigFilePath(void) const;

    bool contextFormatValid(const std::string& line);
    bool gotAnotherServer(std::ifstream& file, std::string& line);
    bool isEndOfConfigFile(std::ifstream& file, std::string& line);
    bool directiveFormatValid(const std::string& line, int indentSize);

    void extractServer(std::ifstream& file, std::string& line, server& server);
    void extractContext(std::ifstream& file, std::string& line, server& server);
    void extractDirective(std::string& line, directiveValue& newDirective, int indentSize);

    void parseConfigFile(void);
    void initConfigParser(void);

    Config(const char*& configFilePath);
    ~Config();

private:
    size_t              _cgiNbr;
    size_t              _cgiTotal;
    size_t              _lineNbr;
    size_t              _locationNbr;
    size_t              _serverIndex;
    std::map<int, int>  _locationNbrMap;
    std::map<int, int>  _cgiNbrMap;
    unsigned int        _contextIndex;
    unsigned int        _directiveIndex;
    configParser        _configParser;
    configFormat        _configFormat;
    const char*&        _configFilePath;
};
