#include "../../include/Config.hpp"

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

#define LOCATION_STRING_LENGTH 9 // location:
#define EXPECTED_DIRECTIVE _configFormat[_contextIndex][_directiveIndex]
#define EXPECTED_CONTEXT _contextNameList[_contextIndex]

const char* _contextNameList[5] = {
    "server:", "location:", "error_page:", "redirection:", "cgi:"};

const char*& Config::getConfigFilePath(void) const {
    return _configFilePath;
}

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
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "line empty while excepting a directive.");

    else if (lineLength < indentSize
          || lineLength < indentSize + static_cast<int>(DIRECTIVE_NAME_LENGTH))
        manageConfigError(line, EXPECTED_DIRECTIVE, "config not well formatted.");

    else if (!rightIndentation(line, indentSize))
        manageConfigError(line, EXPECTED_DIRECTIVE, "wrong indentation.");

    // is directive name format valid
    else if (_configFormat[_contextIndex][_directiveIndex]
            != line.substr(indentSize, DIRECTIVE_NAME_LENGTH))
        manageConfigError(line, EXPECTED_DIRECTIVE, "directive name not valid.");

    std::string tmp = line.substr(indentSize + DIRECTIVE_NAME_LENGTH);
    if (!std::isspace(tmp[0]))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "no space between directive name and directive value.");

    else if (_contextIndex == LOCATION && tmp.length() < 2
        && (!tmp[1] || tmp[1] != '/'))
        manageConfigError(line, EXPECTED_DIRECTIVE, "location directive invalid.");

    else if (!tmp[1] || std::isspace(tmp[1]))
        manageConfigError(line, EXPECTED_DIRECTIVE,
            "directive value not well formatted.");

    return true;
}

void    Config::getDirective(std::string& line, directiveValue& newDirective, int indentSize)
{
    newDirective = line.substr(indentSize + DIRECTIVE_NAME_LENGTH + 1,
        line.length() - indentSize - DIRECTIVE_NAME_LENGTH - 1);
}

void    Config::getContext(std::ifstream& file, std::string& line, server& server)
{
    int indentSize;

    if (_contextIndex == 0)
        indentSize = 4;
    else
        indentSize = 8;
    _directiveIndex = 0;
    while (_directiveIndex < _configFormat[_contextIndex].size()) {
        directiveValue   newDirective;

        if (_contextIndex == LOCATION
            && _directiveIndex == PATH) {
            getDirective(line, newDirective, indentSize);
        }
        else {
            std::getline(file, line);
            if (directiveFormatValid(line, indentSize))
                getDirective(line, newDirective, indentSize);
        }
        server[_contextIndex][_directiveIndex] = newDirective;
        _directiveIndex++;
    }
}

bool    Config::contextFormatValid(const std::string& line)
{
    size_t indentSize = 0;

    if (_contextIndex > 0)
        indentSize = 4;
    if (_contextIndex == LOCATION) {
        if (line.length() <= LOCATION_STRING_LENGTH + indentSize)
            manageConfigError(line, EXPECTED_CONTEXT, "location context format not valid.");
        else
            return true;
    } else if (line.empty()
        || std::strlen(_contextNameList[_contextIndex]) + indentSize != line.length()
        || _contextNameList[_contextIndex] != line.substr(indentSize, line.length() - indentSize))
        manageConfigError(line, EXPECTED_CONTEXT, "context format not valid.");
    return true;
}

void    Config::getServer(std::ifstream& file, std::string& line, server& server)
{
    _contextIndex = SERVER;
    while (_contextIndex < _configFormat.size()) {
        std::getline(file, line);
        if (file.eof())
            break;
        if (_contextIndex == LOCATION && contextFormatValid(line))
            while (contextFormatValid(line)) {
                if (line.substr(4, 9) != "location:")
                    break;
                getContext(file, line, server);
            }
        else if (contextFormatValid(line))
            getContext(file, line, server);
        _contextIndex++;
    }
    std::getline(file, line);
}

bool    gotAnotherServer(std::ifstream& file, std::string& line)
{
    if (file.eof())
        return false;
    else if (!line.empty())
        manageConfigError(line, "", "config file not well formated.");
    std::ifstream::pos_type streamPos = file.tellg();
    std::getline(file, line);
    file.seekg(streamPos);
    return !line.empty();
}

void    Config::parseConfigFile(void)
{
    std::string line;
    std::ifstream file(_configFilePath);

    if (!file)
        manageConfigError(line, "", "can't open config file");
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
    context location;
    context error;
    // context redirection;
    // context cgi;

    _configFormat[SERVER] = server;
    _configFormat[LOCATION] = location;
    _configFormat[ERROR] = error;
    // _configFormat[REDIRECTION] = redirection;
    // _configFormat[CGI] = cgi;

    // serverDirective
    _configFormat[SERVER][LISTEN] = "listen";
    _configFormat[SERVER][HOST] = "host";
    _configFormat[SERVER][SERVER_NAME] = "server_name";
    _configFormat[SERVER][CLIENT_MAX_BODY_SIZE] = "client_max_body_size";

    // locationDirective
    _configFormat[LOCATION][PATH] = "";
    _configFormat[LOCATION][ROOT] = "root";
    _configFormat[LOCATION][INDEX] = "index";
    _configFormat[LOCATION][AUTOINDEX] = "autoindex";
    _configFormat[LOCATION][METHOD] = "method";

    // errorDirective
    _configFormat[ERROR][E_400] = "400";
    //_configFormat[ERROR][E_401] = "401";
    //_configFormat[ERROR][E_402] = "402";
    _configFormat[ERROR][E_403] = "403";
    _configFormat[ERROR][E_404] = "404";

    _configFormat[ERROR][E_500] = "500";
    //_configFormat[ERROR][E_501] = "501";
    //_configFormat[ERROR][E_502] = "502";

    // redirectionDirective
    // _configFormat[REDIRECTION][R_300] = "300";
    // _configFormat[REDIRECTION][R_301] = "301";
    // _configFormat[REDIRECTION][R_302] = "302";

    // cgiDirective
    // _configFormat[CGI][0] = ".php";
    // _configFormat[CGI][1] = ".perl";
    // _configFormat[CGI][2] = ".py";
    // _configFormat[CGI][3] = ".c";
}

Config::Config(const char*& configFilePath)
    : _configFilePath(configFilePath)
{
    initConfigParser();
    // printConfigFormat();
    parseConfigFile();
    // printConfig();
}

Config::~Config() {}
