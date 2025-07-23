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
const int   directiveNbr[5] = {3, 4, 1, 2, 3};

void    Config::printConfigFormat(void) const {
    size_t i = 0;
    std::cout << BOLD ITALIC BLUE << "[ WEBSERV CONFIGURATION FILE FORMAT ]" << DEFAULT << std::endl;
    std::cout << BOLD WHITE << contextNameList[0] << ":" << DEFAULT << std::endl;
    for (contextFormatIterator it = _contextFormat.begin();
                               it != _contextFormat.end(); it++) {
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

void    Config::printServer(server server) {
    for (serverIterator it = server.begin(); it != server.end(); it++) {
        for (contextIterator it2 = it->second.begin(); it2 != it->second.end(); it2 ++) {
            // std::cout << it2->first << std::endl;
            // std::cout << it2->second[0] << std::endl;
            // for (size_t i = 0; i < it2->second.size(); i++) {
            //     std::cout << it2->second[i];
            // }
        }
    }
}

// void    Config::printServer(serverIterator server) {
//     for (contextIterator it2 = server->second.begin(); it2 != server->second.end(); it2 ++) {
//         for (size_t i = 0; i < it2->second.size(); i++) {
//             std::cout << it2->second[i];
//         }
//     }
// }

void    Config::printConfig(void) const {
    server server;

    for (configIterator it = _webservConfig.begin();
                        it != _webservConfig.end(); it++) {
        // printServer(it->begin());
        // printServer(it);
    }
}

bool    isRightIndentation(const std::string& line, uint32_t indentSize) {
    return line.length() > indentSize
        && line.substr(0, indentSize).find_first_not_of(" ");
}

bool    Config::isContextFormatValid(std::string& line) {
    if (line.empty() || contextNameList[_contextIndex] != line)
        throw std::runtime_error("Error: context format not valid.");
    return true;
}

bool    Config::isContextDirectiveFormatValid(std::string& line, int indentSize) {
    size_t directiveNameLength = _contextFormat[_contextIndex][_directiveIndex][0].length();
    if (line.empty() || line.length() - indentSize < directiveNameLength
        || _contextFormat[_contextIndex][_directiveIndex][0] != line.substr(indentSize, directiveNameLength))
        throw std::runtime_error("Error: directive format not valid.");
    return true;
}

bool    Config::isEndOfConfigFile(std::ifstream& file, std::string& line) {
    return line.empty() && file.eof();
}

void    Config::checkDirectiveFormat(const std::string& line) {
    static uint8_t directiveIndex;

    (void)line;
    (void)directiveIndex;
}

void    Config::checkContextFormat(const std::string& line) {
    (void)line;
}

void    Config::getContextDirective(std::string& line, directive& newDirective, int indentSize) {
    if (_serverMask & 1 << _directiveIndex)
        throw std::runtime_error("Error: got doublon in config file.");
    else if ((_contextIndex == 0 && !isRightIndentation(line, indentSize))
                || !isRightIndentation(line, indentSize))
        throw std::runtime_error("Error: wrong indentation in config file.");
    if (_contextFormat[_contextIndex][_directiveIndex][FORMAT] == line)
        newDirective.push_back(line.substr());
    else
        newDirective.push_back("");
    _serverMask |= 1 << _directiveIndex;
}

void    Config::getContext(std::ifstream& file, std::string& line, server& server) {
    int indentSize;

    _directiveIndex = 0;
    if (_contextIndex == 0)
        indentSize = 4;
    else
        indentSize = 8;
    while (_directiveIndex < directiveNbr[_contextIndex]) {
        directive newDirective;
        std::getline(file, line);
        if (line.empty())
            break;
        if (isContextDirectiveFormatValid(line, indentSize)) {
            getContextDirective(line, newDirective, indentSize);
            // std::cout << line << std::endl;
        }
        server[_contextIndex][_directiveIndex] = newDirective;
        _directiveIndex++;
    }
}

void    Config::getServer(std::ifstream& file, std::string& line, server& server) {
    _serverMask = 0;
    _contextIndex = SERVER;
    while (_contextIndex < CONTEXT_NUMBER) {
        std::getline(file, line);
        if (isContextFormatValid(line)) {
            getContext(file, line, server);
        }
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
    std::string line;
    std::ifstream file(_configFilePath.c_str());

    if (!file)
        throw std::runtime_error("Error: can't open config file");
    while (GETTING_ALL_SERVERS) {
        server  server;

        getServer(file, line, server);
        // printServer(server);
        _webservConfig.push_back(server);
        if (!got_an_other_server(file, line))
            break;
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

    _contextFormat[SERVER] = server;
    _contextFormat[ERROR] = error;
    _contextFormat[REDIRECTION] = redirection;
    _contextFormat[LOCATION] = location;
    _contextFormat[CGI] = cgi;

    // serverDirective
    _contextFormat[SERVER][SERVER_NAME].push_back("server_name");
    _contextFormat[SERVER][LISTEN].push_back("listen");
    _contextFormat[SERVER][CLIENT_MAX_BODY_SIZE].push_back("client_max_body_size");

    // errorDirective
    _contextFormat[ERROR][E_400].push_back("400");
    _contextFormat[ERROR][E_401].push_back("401");
    _contextFormat[ERROR][E_402].push_back("402");

    _contextFormat[ERROR][E_500].push_back("500");
    _contextFormat[ERROR][E_501].push_back("501");
    _contextFormat[ERROR][E_502].push_back("502");

    // redirectionDirective
    _contextFormat[REDIRECTION][R_300].push_back("300");
    _contextFormat[REDIRECTION][R_301].push_back("301");
    _contextFormat[REDIRECTION][R_302].push_back("302");

    // locationDirective
    _contextFormat[LOCATION][0].push_back("");
    _contextFormat[LOCATION][1].push_back("");
    _contextFormat[LOCATION][2].push_back("");

    // cgiDirective
    _contextFormat[CGI][0].push_back("");
    _contextFormat[CGI][1].push_back("");
    _contextFormat[CGI][2].push_back("");
}

Config::Config(const char* configFilePath) : _configFilePath(configFilePath) {
    initConfigParser();
    // printConfigFormat();
    parseConfigFile();
    // printConfig();
}

Config::~Config() {}
