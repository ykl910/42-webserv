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

// const char* _contextNameList[5] = {
//     "server:", "location:", "error_page:", "redirection:", "cgi:"};

// const char* _contextNameList[5] = {
//     "server:", "location:", "error_page:", "redirection:", "cgi:"};

const char*& Config::getConfigFilePath(void) const {
    return _configFilePath;
}

configParser&   Config::getConfigParser(void) {
    return _configParser;
}

size_t  Config::getLocationNbr(size_t i)
{
    return _locationNbrMap[i];
}

size_t  Config::getCgiNbr(size_t i)
{
    return _cgiNbrMap[i];
}

size_t Config::getCgiTotal(void)
{
    return _cgiTotal;
}

bool    Config::rightIndentation(const std::string& line, uint32_t indentSize) {
    return line.length() > indentSize
        && line.substr(0, indentSize).find_first_not_of(" ");
}

bool    Config::isEndOfConfigFile(std::ifstream& file, std::string& line)
{
    return line.empty() && file.eof();
}

bool    Config::gotAnotherServer(std::ifstream& file, std::string& line)
{
    if (file.eof())
        return false;

    std::ifstream::pos_type streamPos = file.tellg();
    size_t i = _lineNbr;
    while (std::getline(file, line)) {
        if (line == "server:") {
            file.seekg(streamPos);
            return true;
        } else if (line.length() == 0) {
            streamPos = file.tellg();
        } else if (line.length() > 0 && line != "server:")
            manageConfigError(line, "", "format between servers not valid.", i);
        ++i;
    }
    if (file.eof() || line.length() == 0)
        return false;
    return true;
}

void    Config::parseConfigFile(void)
{
    std::string line;
    std::ifstream file(_configFilePath);

    if (!file)
        manageConfigError(line, "", "can't open config file", _lineNbr);
    _serverIndex = 0;
    while (GETTING_ALL_SERVERS) {
        server  server;

        extractContext(file, line, server);
        _configParser.push_back(server);
        if (!gotAnotherServer(file, line))
            break;
        _serverIndex++;
    }
    file.close();
}

void    Config::initConfigFormat(void)
{
    context server;
    context location;
    context error;
    context redirection;
    context cgi;

    _configFormat[SERVER] = server;
    _configFormat[LOCATION] = location;
    _configFormat[ERROR] = error;
    _configFormat[REDIRECTION] = redirection;
    _configFormat[CGI] = cgi;

    // serverDirective
    _configFormat[SERVER][LISTEN].push_back("listen");
    _configFormat[SERVER][HOST].push_back("host");
    _configFormat[SERVER][SERVER_NAME].push_back("server_name");
    _configFormat[SERVER][CLIENT_MAX_BODY_SIZE].push_back("client_max_body_size");
    _configFormat[SERVER][ROOT_LOCATION].push_back("root");

    // locationDirective
    _configFormat[LOCATION][PATH].push_back("");
    _configFormat[LOCATION][ROOT].push_back("root");
    _configFormat[LOCATION][INDEX].push_back("index");
    _configFormat[LOCATION][AUTOINDEX].push_back("autoindex");
    _configFormat[LOCATION][METHOD].push_back("method");

    // errorDirective
    _configFormat[ERROR][E_400].push_back("400");
    _configFormat[ERROR][E_403].push_back("403");
    _configFormat[ERROR][E_404].push_back("404");
    _configFormat[ERROR][E_405].push_back("405");
    _configFormat[ERROR][E_500].push_back("500");
    _configFormat[ERROR][E_501].push_back("501");

    // redirectionDirective
    _configFormat[REDIRECTION][R_301].push_back("301");
    _configFormat[REDIRECTION][R_302].push_back("302");

}

Config::Config(const char*& configFilePath)
    : _cgiNbr(0), _cgiTotal(0), _lineNbr(0),
    _locationNbr(0), _configFilePath(configFilePath)
{
    initConfigFormat();
    parseConfigFile();
}

Config::~Config() {}
