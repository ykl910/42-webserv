#include "../include/Config.hpp"
#include "../include/WebServ.hpp"

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

#define CONTEXT_NUMBER 1
#define GETTING_ALL_SERVERS 1

const char* contextNameList[] = {"server:", "error_page:", "redirection:", "location:", "cgi:"};
const int   directiveNbr[5]   = {4, 4, 1, 2, 3};

void    Config::printConfigFormat(void) const
{
    std::cout
    << BOLD ITALIC BLUE << "[ WEBSERV CONFIGURATION FILE FORMAT ]\n"
    << BOLD WHITE << contextNameList[0] << DEFAULT << std::endl;

    size_t i = 0;
    for (configFormatIterator it = _configFormat.begin();
                               it != _configFormat.end(); ++it) {
        if (i != 0)
            std::cout << "    "
            << BOLD WHITE << contextNameList[i] << DEFAULT << std::endl;
        for (contextIterator it2 = it->second.begin();
                             it2 != it->second.end(); ++it2) {
                for (size_t j = 0; j < it2->second.size(); j++) {
                    if (i == 0)
                        std::cout << "    " << it2->second[j] << std::endl;
                    else
                        std::cout << "        " << it2->second[j] << std::endl;
                }
        }
        ++i;
    }
}

void    Config::printServer(const server& srv) const {
    for (serverIterator it = srv.begin(); it != srv.end(); ++it) {
        for (contextIterator it2 = it->second.begin();
                             it2 != it->second.end(); ++it2) {
            // std::cout << it2->first << std::endl;
            // std::cout << it2->second[0] << std::endl;
            // for (size_t i = 0; i < it2->second.size(); i++) {
            //     std::cout << it2->second[i];
            // }
        }
    }
}

void    Config::printConfig(void) const {
    for (configIterator it = _webservConfig.begin();
                        it != _webservConfig.end(); ++it) {
        printServer(*it);
    }
}

bool    rightIndentation(const std::string& line, uint32_t indentSize) {
    return line.length() > indentSize
        && line.substr(0, indentSize).find_first_not_of(" ");
}

bool    Config::contextFormatValid(std::string& line)
{
    if (line.empty() || contextNameList[_contextIndex] != line)
        throw std::runtime_error("Error: context format not valid.");
    return true;
}

bool    Config::contextDirectiveFormatValid(std::string& line, int indentSize)
{
    size_t directiveNameLength = _configFormat[_contextIndex][_directiveIndex].length();
    if (line.empty() || line.length() - indentSize < directiveNameLength
        || _configFormat[_contextIndex][_directiveIndex]
            != line.substr(indentSize, directiveNameLength))
        throw std::runtime_error("Error: directive format not valid.");
    return true;
}

bool    Config::isEndOfConfigFile(std::ifstream& file, std::string& line)
{
    return line.empty() && file.eof();
}

bool    Config::directiveFormatValid(const std::string& line)
{
    (void)line;
    // if (line.)
    return true;
}

void    Config::getContextDirective(std::string& line, directive& newDirective,
                                    int indentSize)
{
    if (_serverMask & 1 << _directiveIndex)
        throw std::runtime_error("Error: got doublon in config file.");
    else if (!rightIndentation(line, indentSize))
        throw std::runtime_error("Error: wrong indentation in config file.");
    if (_configFormat[_contextIndex][_directiveIndex] == line)
        newDirective = line.substr();
    else
        newDirective = "";
    _serverMask |= 1 << _directiveIndex;
}

void    Config::getContext(std::ifstream& file, std::string& line, server& server)
{
    int indentSize;

    _directiveIndex = 0;
    if (_contextIndex == 0)
        indentSize = 4;
    else
        indentSize = 8;
    while (_directiveIndex < directiveNbr[_contextIndex]) {
        directive newDirective;
        std::getline(file, line);
        std::cout << line << "\n";
        if (line.empty())
            break;
        if (contextDirectiveFormatValid(line, indentSize)) {
            getContextDirective(line, newDirective, indentSize);
            // std::cout << line << std::endl;
        }
        server[_contextIndex][_directiveIndex] = newDirective;
        _directiveIndex++;
    }
}

void    Config::getServer(std::ifstream& file, std::string& line, server& server)
{
    _serverMask = 0;
    _contextIndex = SERVER;
    while (_contextIndex < CONTEXT_NUMBER) {
        std::getline(file, line);
        if (contextFormatValid(line)) {
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
        _webservConfig.push_back(server);
        printConfig();
        if (!gotAnotherServer(file, line))
            break;
    }
    file.close();
}

const char* Config::getConfigFilePath(void) const
{
    return _configFilePath.c_str();
}

void    Config::initConfigParser(void)
{
    context server;
    // context error;
    // context redirection;
    // context location;
    // context cgi;

    _configFormat[SERVER] = server;
    // _configFormat[ERROR] = error;
    // _configFormat[REDIRECTION] = redirection;
    // _configFormat[LOCATION] = location;
    // _configFormat[CGI] = cgi;

    // serverDirective
    _configFormat[SERVER][SERVER_NAME] = "server_name";
    _configFormat[SERVER][LISTEN] = "listen";
    // _configFormat[SERVER][CLIENT_MAX_BODY_SIZE] = "client_max_body_size";

    // errorDirective
    // _configFormat[ERROR][E_400] = "400";
    // _configFormat[ERROR][E_401] = "401";
    // _configFormat[ERROR][E_402] = "402";

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

Config::Config(const char* configFilePath) : _configFilePath(configFilePath)
{
    // initConfigParser();
    // printConfigFormat();
    // parseConfigFile();
    // printConfig();
    // exit(0);
}

Config::~Config() {}
