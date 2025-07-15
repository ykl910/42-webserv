#include "../include/Config.hpp"
#include "../include/WebServ.hpp"

/*
Context:
    - TOP LEVEL: main, events, http, stream, mail
    - HTTP: server, upstream, map, geo, types, limite_req_zone, limit_conn_zone,
log_format, include, charset_map, server_names_hash_bucket_size
    - SERVER: location, if, limit_except, include
    - LOCATION: if, limit_except, include
    - STREAM: server, upstream
    - MAIL:

Directives:
    -
*/

void    Config::getNextDirective(const std::string& line) {
    (void)line;
}

void    Config::getNextContext(const std::string& context) {
    (void)context;
}

void    Config::parseConfigFile(void) {
    std::ifstream file(this->_configPath.c_str());

    if (!file)
        throw std::runtime_error("Error: can't open config file");
    std::string line;
    while (std::getline(file, line)) {
        getNextContext(line);
        // std::cout << line << std::endl;
    }
}

Config::Config() : _configPath("config/webserv.conf") {
    this->parseConfigFile();
}

Config::Config(const std::string& configPath) : _configPath(configPath) {
    this->parseConfigFile();
}

Config::~Config() {}
