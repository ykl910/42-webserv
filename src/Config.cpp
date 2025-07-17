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

bool    isDirective(const std::string& line) {
    return line == "listen";
}

bool    isContext(const std::string& line) {
    return line == "http" || line == "server";
}

void    Config::getNextDirective(const std::string& line) {
    (void)line;
}

void    Config::getNextContext(const std::string& context) {
    (void)context;
}

void    Config::getMainContext(const std::string& context) {
    for (size_t i = 0; context[i]; i++)
        (void)context;
}

void    Config::getServerContext(const std::string& context) {
    for (size_t i = 0; context[i]; i++)
        (void)context;
}

void    Config::getHttpContext(const std::string& context) {
    for (size_t i = 0; context[i]; i++)
        (void)context;
}

bool    isHttpContext(const std::string& line) {
    (void)line;
    return true;
}

void    Config::parseConfigFile(void) {
    std::ifstream file(_configPath.c_str());

    if (!file)
        throw std::runtime_error("Error: can't open config file");

    std::string line;
    // int i = 0;
    while (std::getline(file, line)) {

        // if (isHttpContext(line) && _directive.mainMask & )
        // while (std::isspace(line[i++]))
        // if ()
        // getMainContext();
    }
}

Config::Config() : _configPath("config/webserv.conf") {
    parseConfigFile();
}

Config::Config(const std::string& configPath) : _configPath(configPath) {
    parseConfigFile();
}

Config::~Config() {}
