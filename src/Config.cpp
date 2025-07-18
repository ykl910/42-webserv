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

uint64_t server_nbr = 0;

bool    Config::isDirective(const std::string& line) {
    return line == "listen";
}

bool    Config::isDirective(const directiveIt& it) {
    (void)it;
    return true;
}

bool    isContext(const std::string& line) {
    return line == "http" || line == "server";
}

bool    isServerContext(std::ifstream& file, std::string& line) {
    while (std::getline(file, line)) {
        (void)line;
        // std::cout << line << std::endl;
    }
    return true;
}

bool    isHttpContext(std::ifstream& file, std::string& line) {
    size_t i = 0;

    while (line[i] && std::isspace(line[i])) { ++i; }
    if (line.substr(i, 4) != "http")
    return false;

    i += 5;
    while (line[i] && line[i] != '{') {
        std::cout << line[i];
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
    std::cout << "Here\n";
    return false;
}

void    Config::printConfig(void) {
    return;
    // for (mainContextIt it = _mainContext.begin(); it != _mainContext.end(); ++it) {
    //     if (isDirective(it->second)) {
    //         for (directiveIt it = it->second.begin(); it != it->second.end(); ++it) {

    //         }
    //     } else {
    //         for (httpContext it = _httpContext.begin(); it != _httpContext.end(); ++it) {

    //         }
    //     }

    //     }
}

// void    Config::getNextDirective(std::ifstream& file, Context& context, std::string& line) {
//     (void)line;
//     (void)file;
//     (void)context;
// }

void    Config::getNextContext(std::ifstream& file, context& context, std::string& line) {
    (void)line;
    (void)file;
    (void)context;
}

void    Config::getMainContext(std::ifstream& file, std::string& line) {
    (void)file;
    for (size_t i = 0; line[i]; i++)
        (void)line;
}

void    Config::getServerContext(std::ifstream& file, std::string& line) {
    (void)file;
    std::cout << BOLD MAGENTA ITALIC
    << "        SERVER CONTEXT " << server_nbr + 1 << "\n" << DEFAULT;
    for (size_t i = 0; line[i]; i++)
        (void)line;
}

void    Config::getHttpContext(std::ifstream& file, std::string& line) {
    (void)file;
    (void)line;
    std::cout << BOLD BLUE ITALIC << "    HTTP CONTEXT\n" << DEFAULT;
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
    std::cout << BOLD WHITE ITALIC << "MAIN CONTEXT\n" << DEFAULT;
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
