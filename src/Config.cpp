#include "../include/Config.hpp"

/* Nginx context tree
main
├── events
│   └── (no subcontexts, only directives)

├── http
│   ├── server
│   │   ├── location
│   │   │   ├── if
│   │   │   └── limit_except
│   │   ├── if          (also valid directly inside server)
│   │   └── limit_except
│   ├── upstream
│   ├── map
│   ├── geo
│   ├── types
│   ├── charset_map
│   ├── log_format
│   ├── limit_conn_zone
│   ├── limit_req_zone
│   ├── include
│   └── (various directives like sendfile, access_log, etc.)

├── stream               (if compiled with --with-stream)
│   ├── server
│   │   └── proxy_pass
│   ├── upstream
│   ├── map
│   ├── geo
│   ├── log_format
│   ├── limit_conn_zone
│   └── include

├── mail                 (if compiled with --with-mail)
│   ├── server
│   ├── auth_http
│   ├── proxy
│   └── include

├── load_module          (directive, not a block)
└── include              (directive, includes external config)
*/

const char* _contextNameList[5] = {
    "server:", "error_page:", "redirection:", "location:", "cgi:"};

const int   _directiveNbr[5] = {4, 4, 1, 2, 3};

configParser&   Config::getConfigParser(void) {
    return _configParser;
}

bool    rightIndentation(const std::string& line, uint32_t indentSize) {
    return line.length() > indentSize
        && line.substr(0, indentSize).find_first_not_of(" ");
}

bool    Config::isEndOfConfigFile(std::ifstream& file, std::string& line)
{
    return line.empty() && file.eof();
}

bool    Config::directiveFormatValid(const std::string& line, int indentSize)
{
    int lineLength = line.length();

    if (lineLength == 0)
        throw std::runtime_error(
            "Error config: line empty while excepting a directive.");

    else if (lineLength < indentSize
          || lineLength < indentSize + static_cast<int>(DIRECTIVE_NAME_LENGTH))
        throw std::runtime_error("Error config: config not well formatted.");

    else if (!rightIndentation(line, indentSize))
        throw std::runtime_error("Error config: wrong indentation.");

    // is directive name format valid
    else if (_configFormat[_contextIndex][_directiveIndex]
            != line.substr(indentSize, DIRECTIVE_NAME_LENGTH))
        throw std::runtime_error("Error config: directive name not valid.");

    std::string tmp = line.substr(indentSize + DIRECTIVE_NAME_LENGTH);
    if (!std::isspace(tmp[0]))
        throw std::runtime_error(
            "Error config: no space between directive name and directive value.");
    else if (!tmp[1] || std::isspace(tmp[1]))
        throw std::runtime_error(
            "Error config: directive value not well formatted.");
    return true;
}

void    Config::getDirective(std::string& line, directive& newDirective, int indentSize)
{
    if (_serverMask & 1 << _directiveIndex)
        throw std::runtime_error(
            "Error: config: got already this directive in this context.");

    newDirective = line.substr(indentSize + DIRECTIVE_NAME_LENGTH + 1,
        line.length() - indentSize - DIRECTIVE_NAME_LENGTH - 1);
    _serverMask |= 1 << _directiveIndex;
}

void    Config::getContext(std::ifstream& file, std::string& line, server& server)
{
    int indentSize;

    if (_contextIndex == 0)
        indentSize = 4;
    else
        indentSize = 8;
    _directiveIndex = 0;
    while (_directiveIndex < _directiveNbr[_contextIndex]) {
        directive   newDirective;

        std::getline(file, line);
        if (directiveFormatValid(line, indentSize))
            getDirective(line, newDirective, indentSize);
        server[_contextIndex][_directiveIndex] = newDirective;
        _directiveIndex++;
    }
}

bool    Config::contextFormatValid(const std::string& line)
{
    int indentSize = 0;

    if (_contextIndex > 0)
        indentSize = 4;
    if (line.empty()
        || std::strlen(_contextNameList[_contextIndex]) + indentSize != line.length()
        || _contextNameList[_contextIndex] != line.substr(indentSize, line.length() - indentSize))
        throw std::runtime_error("Error: context format not valid.");
    return true;
}

void    Config::getServer(std::ifstream& file, std::string& line, server& server)
{
    _contextIndex = SERVER;
    // while (_contextIndex < CONTEXT_NUMBER) {
    while (1) {
        _serverMask = 0;
        std::getline(file, line);
        if (file.eof())
            break;
        else if (contextFormatValid(line)) {
            getContext(file, line, server);
        }
        _contextIndex++;
    }
}

bool    gotAnotherServer(std::ifstream& file, std::string& line)
{
    std::getline(file, line);
    if (!line.empty())
        throw std::runtime_error("Error: config file not well formated.");
    std::ifstream::pos_type streamPos = file.tellg();
    std::getline(file, line);
    file.seekg(streamPos);
    return !line.empty();
}

void    Config::parseConfigFile(void)
{
    std::string line;
    std::ifstream file(_configFilePath.c_str());

    if (!file)
        throw std::runtime_error("Error: can't open config file");
    while (GETTING_ALL_SERVERS) {
        server  server;

        getServer(file, line, server);
        _configParser.push_back(server);
        if (!gotAnotherServer(file, line))
            break;
    }
    file.close();
}

void    Config::initConfigParser(void)
{
    context server;
    context error;
    context redirection;
    context location;
    context cgi;

    _configFormat[SERVER] = server;
    _configFormat[ERROR] = error;
    _configFormat[REDIRECTION] = redirection;
    _configFormat[LOCATION] = location;
    _configFormat[CGI] = cgi;

    // serverDirective
    _configFormat[SERVER][LISTEN] = "listen";
    _configFormat[SERVER][HOST] = "host";
    _configFormat[SERVER][SERVER_NAME] = "server_name";
    _configFormat[SERVER][CLIENT_MAX_BODY_SIZE] = "client_max_body_size";

    // errorDirective
    _configFormat[ERROR][E_400] = "400";
    _configFormat[ERROR][E_401] = "401";
    _configFormat[ERROR][E_402] = "402";
    _configFormat[ERROR][E_403] = "403";
    _configFormat[ERROR][E_404] = "404";

    // _configFormat[ERROR][E_500] = "500";
    // _configFormat[ERROR][E_501] = "501";
    // _configFormat[ERROR][E_502] = "502";

    // redirectionDirective
    // _configFormat[REDIRECTION][R_300] = "300";
    // _configFormat[REDIRECTION][R_301] = "301";
    // _configFormat[REDIRECTION][R_302] = "302";

    // locationDirective
    // _contextFormat[LOCATION][0].push_back("");

    // cgiDirective
    // _contextFormat[CGI][0].push_back(".php");
    // _contextFormat[CGI][1].push_back(".perl");
    // _contextFormat[CGI][2].push_back(".py");
    // _contextFormat[CGI][3].push_back(".c");
}

Config::Config(const char* configFilePath)
    : _configFilePath(configFilePath)
{
    initConfigParser();
    // printConfigFormat();
    parseConfigFile();
    // printConfigParser();
}

Config::~Config() {}
