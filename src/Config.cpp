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


bool    serverContextNameValid(const std::string& line) {
    return line == "server:";
}

bool    locationContextNameValid(const std::string& line) {
    return line == "location:";
}

bool    errorContextNameValid(const std::string& line) {
    return line == "error:";
}

bool    isRightIndentation(const std::string& line, uint8_t indentSize) {
    return line.size() > indentSize
        && line.substr(0, indentSize).find_first_not_of(" ");
}

void    Config::getServerLocationContext(std::ifstream& file,
            std::string& line, t_server& server, uint8_t directiveNbr) {

    while (std::getline(file, line)) {
        (void)line;
        (void)server;
        (void)directiveNbr;
    }
}

std::string    getServerDirectiveName(uint8_t directiveNbr) {
    std::string result;
    switch (directiveNbr) {
        case SERVER_NAME:
            return result = std::string("server_name");
        case LISTEN:
            return result = std::string("listen");
        case CLIENT_MAX_BODY_SIZE:
            return result = std::string("client_max_body_size");
        default:
            return result = std::string(" ");
    }
}

void    Config::getServerDirective(
            std::string& line, t_server& server, uint8_t directiveNbr) {
    if (server.mask & 1 << directiveNbr)
        throw std::runtime_error("Error: got doublon in config file.");
    else if (!isRightIndentation(line, 4))
        throw std::runtime_error("Error: wrong indentation in config file.");
    t_directive directive;
    directive.name = getServerDirectiveName(directiveNbr);
    if (directive.name == line.substr(5, directive.name.length())
        && line[directive.name.length() + 1] == ' ')
    server.mask |= 1 << directiveNbr;
    server.directiveList.push_back(directive);
}

void    Config::getServerContext(std::ifstream& file, std::string& line) {
    t_server    server;
    uint8_t     directiveNbr = 0;
    bool        isLocation = false;

    server.mask = 0;
    while (std::getline(file, line)) {
        if (line.empty())
            break;
        else if (!isLocation) {
            getServerDirective(line, server, directiveNbr);
            directiveNbr++;
            // isLocation = true;
        } else
            getServerLocationContext(file, line, server, directiveNbr);
    }
    _webservConfig.serverList.push_back(server);
}

void    Config::printServerConfig(void) {
    for (size_t i = 0; i < _webservConfig.serverList.size(); i++) {
        for (size_t j = 0; j < _webservConfig.serverList[i].directiveList.size(); j++) {
            std::cout << _webservConfig.serverList[i].directiveList[j].name << std::endl;
            std::cout << _webservConfig.serverList[i].directiveList[j].argument << std::endl;
        }
    }
}

void    Config::parseConfigFile(directive& directiveList) {
    std::ifstream file(_configFilePath.c_str());

    (void)directiveList;
    if (!file)
        throw std::runtime_error("Error: can't open config file");
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && serverContextNameValid(line))
            getServerContext(file, line);
    }
    file.close();
}

const char* Config::getConfigFilePath(void) const {
    return _configFilePath.c_str();
}

void    Config::initConfig(directive& directiveList) {

    // serverDirective
    directiveList[SERVER][0] = "server_name";
    directiveList[SERVER][1] = "listen";
    directiveList[SERVER][2] = "client_max_body_size";

    // errorDirective
    directiveList[ERROR_PAGE][0] = "";
    directiveList[ERROR_PAGE][1] = "";
    directiveList[ERROR_PAGE][2] = "";

    // returnDirective
    directiveList[RETURN][0] = "";
    directiveList[RETURN][1] = "";
    directiveList[RETURN][2] = "";

    // locationDirective
    directiveList[LOCATION][0] = "";
    directiveList[LOCATION][1] = "";
    directiveList[LOCATION][2] = "";

    // cgiDirective
    directiveList[CGI][0] = "";
    directiveList[CGI][1] = "";
    directiveList[CGI][2] = "";

}

Config::Config(const char* configFilePath) : _configFilePath(configFilePath) {
    directive directiveList;

    std::map<uint8_t, std::string>  serverDirective;
    std::map<uint8_t, std::string>  errorDirective;
    std::map<uint8_t, std::string>  returnDirective;
    std::map<uint8_t, std::string>  locationDirective;
    std::map<uint8_t, std::string>  cgiDirective;

    directiveList[0] = serverDirective;
    directiveList[1] = errorDirective;
    directiveList[2] = returnDirective;
    directiveList[3] = locationDirective;
    directiveList[4] = cgiDirective;

    initConfig(directiveList);
    for (size_t i = 0; i < serverDirective.size(); i++)
        std::cout << serverDirective[i] << std::endl;
    parseConfigFile(directiveList);
    // exit(0);
}

Config::~Config() {}
