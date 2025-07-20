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

uint64_t server_nbr = 0;

bool    Config::isDirective(const std::string& line) {
    return line == "listen";
}

bool    isContext(const std::string& line) {
    return line == "http" || line == "server";
}

bool    isServerContext(std::ifstream& file, std::string& line) {
    size_t i = 0;

    while (line[i] && std::isspace(line[i])) { ++i; }
    if (line.substr(i, 6) != "server")
    return false;

    i += 5;
    while (line[i] && line[i] != '{') {
        if (!std::isspace(line[i]) || line[i] != '{')
        return false;
        i++;
    }
    std::streampos offset = file.tellg();
    while (std::getline(file, line)) {
        if (line.find("}")) {
            file.seekg(offset);
            return true;
        }
    }
    return false;
}

bool    isHttpContext(std::ifstream& file, std::string& line) {
    size_t i = 0;

    while (line[i] && std::isspace(line[i])) { ++i; }
    if (line.substr(i, 4) != "http")
    return false;

    i += 5;
    while (line[i] && line[i] != '{') {
        if (!std::isspace(line[i]) || line[i] != '{')
        return false;
        i++;
    }
    std::streampos offset = file.tellg();
    while (std::getline(file, line)) {
        if (line.find("}")) {
            file.seekg(offset);
            return true;
        }
    }
    return false;
}

void    Config::printConfig(void) {
    
    // std::cout << BOLD WHITE ITALIC << "MAIN CONTEXT\n" << DEFAULT;
    return;
}

void    Config::getMainContext(std::ifstream& file, std::string& line) {
    (void)file;
    for (size_t i = 0; line[i]; i++)
        (void)line;
}

void    Config::getServerContext(std::ifstream& file, std::string& line) {
    (void)file;
    // std::cout << BOLD MAGENTA ITALIC
    // << "        SERVER CONTEXT " << server_nbr + 1 << "\n" << DEFAULT;
    for (size_t i = 0; line[i]; i++)
        (void)line;

}

void    Config::getHttpContext(std::ifstream& file, std::string& line) {
    (void)file;
    (void)line;
    // std::cout << BOLD BLUE ITALIC << "    HTTP CONTEXT\n" << DEFAULT;
    while (std::getline(file, line)) {
        if (isServerContext(file, line)) {
            getServerContext(file, line);
            // exit(0);
        }
    }
}

void    Config::parseConfigFile(void) {
    std::ifstream file(_configPath.c_str());

    if (!file)
        throw std::runtime_error("Error: can't open config file");

    std::string line;
    while (std::getline(file, line)) {
        if (isHttpContext(file, line)) {
            if (!(_configMask.mainContext & HTTP_CONTEXT)) {
                _configMask.mainContext |= 1 << HTTP;
                getHttpContext(file, line);
            } else
                throw std::runtime_error("Error: config file have more than one HTTP context");
        }
            // getNextDirective(file, _mainContext, line);
    }
    file.close();
}

Config::Config() : _configPath("config/webserv.conf") {
    parseConfigFile();
    // exit(0);
}

Config::Config(const std::string& configPath) : _configPath(configPath) {
    parseConfigFile();
    // exit(0);
}

Config::~Config() {}
