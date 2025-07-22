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

#define CONTEXT_NBR 5

const char* contextNameList[] = {"server", "error_page", "redirection", "location", "cgi"};
const int   directiveNbr[5] = {3, 4, 1, 2, 3};

void    Config::printConfigSyntax(void) const {
    size_t i = 0;
    std::cout << BOLD ITALIC BLUE << "[ WEBSERV CONFIGURATION FILE FORMAT ]" << DEFAULT << std::endl;
    std::cout << BOLD WHITE << contextNameList[0] << ":" << DEFAULT << std::endl;
    for (contextListIterator it = _contextList.begin();
                             it != _contextList.end(); it++) {
        if (i != 0)
            std::cout << "    " << BOLD WHITE << contextNameList[i] << ":" << DEFAULT << std::endl;
        for (contextIterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                for (size_t j = 0; j < it2->second.size(); j++) {
                    if (i == 0)
                        std::cout << "    " << it2->second[j] << std::endl;
                    else
                        std::cout << "        " << it2->second[j] << std::endl;
                }
        }
        i++;
    }
}

// void    Config::printServerConfig(void) const {
// }

bool    isRightIndentation(const std::string& line, uint8_t indentSize) {
    return line.size() > indentSize
        && line.substr(0, indentSize).find_first_not_of(" ");
}

void    Config::checkDirectiveFormat(const std::string& line) {
    static uint8_t directiveIndex;

    (void)line;
    (void)directiveIndex;
}

void    Config::checkContextFormat(const std::string& line) {
    (void)line;
}

void    Config::getContextDirective(std::string& line, directive& newDirective) {
    std::cout << line << std::endl;
    if (_serverMask & 1 << _directiveIndex)
        throw std::runtime_error("Error: got doublon in config file.");
    else if ((_contextIndex == 0 && !isRightIndentation(line, 4))
                || !isRightIndentation(line, 8))
        throw std::runtime_error("Error: wrong indentation in config file.");
    std::cout << line << std::endl;
    if (_contextList[_contextIndex][_directiveIndex][FORMAT] == line)
        newDirective.push_back(line);
    else
        newDirective.push_back("");
    _serverMask |= 1 << _directiveIndex;
}

void    Config::getContext(std::ifstream& file, std::string& line, server& server) {

    _directiveIndex = 0;
    while (_directiveIndex < directiveNbr[_contextIndex]) {
        directive newDirective;
        std::getline(file, line);
        std::cout << "context: " << _contextIndex << " | directive: " << _directiveIndex << std::endl;
        if (line.empty())
            break;
        getContextDirective(line, newDirective);
        server[_contextIndex][_directiveIndex] = newDirective;
        _directiveIndex++;
    }
}

void    Config::getServer(std::ifstream& file, std::string& line, server& server) {
    _serverMask = 0;
    _contextIndex = SERVER;
    while (_contextIndex < CONTEXT_NBR) {
        getContext(file, line, server);
        _contextIndex++;
    }
}

bool    got_an_other_server(std::ifstream& file, std::string& line) {
    std::getline(file, line);
    if (!line.empty())
        throw std::runtime_error("Error: config file not well formated.");
    std::getline(file, line);
    return !line.empty();
}

void    Config::parseConfigFile(void) {
    std::ifstream file(_configFilePath.c_str());
    if (!file)
        throw std::runtime_error("Error: can't open config file");

    std::string line;
    while (1) {
        server  server;

        getServer(file, line, server);
        if (!got_an_other_server(file, line))
            break;
        _webservConfig.push_back(server);
    }
    file.close();
}

const char* Config::getConfigFilePath(void) const {
    return _configFilePath.c_str();
}

void    Config::initConfigParser(void) {
    context server;
    context error;
    context redirection;
    context location;
    context cgi;

    _contextList[SERVER] = server;
    _contextList[ERROR] = error;
    _contextList[REDIRECTION] = redirection;
    _contextList[LOCATION] = location;
    _contextList[CGI] = cgi;

    // serverDirective
    _contextList[SERVER][SERVER_NAME].push_back("server_name");
    _contextList[SERVER][LISTEN].push_back("listen");
    _contextList[SERVER][CLIENT_MAX_BODY_SIZE].push_back("client_max_body_size");

    // errorDirective
    _contextList[ERROR][E_400].push_back("400");
    _contextList[ERROR][E_401].push_back("401");
    _contextList[ERROR][E_402].push_back("402");

    _contextList[ERROR][E_500].push_back("500");
    _contextList[ERROR][E_501].push_back("501");
    _contextList[ERROR][E_502].push_back("502");

    // redirectionDirective
    _contextList[REDIRECTION][R_300].push_back("300");
    _contextList[REDIRECTION][R_301].push_back("301");
    _contextList[REDIRECTION][R_302].push_back("302");

    // locationDirective
    _contextList[LOCATION][0].push_back("");
    _contextList[LOCATION][1].push_back("");
    _contextList[LOCATION][2].push_back("");

    // cgiDirective
    _contextList[CGI][0].push_back("");
    _contextList[CGI][1].push_back("");
    _contextList[CGI][2].push_back("");
}

Config::Config(const char* configFilePath) : _configFilePath(configFilePath) {
    initConfigParser();
    // printConfigSyntax();
    // parseConfigFile();
}

Config::~Config() {}
