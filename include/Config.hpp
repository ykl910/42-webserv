#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>

template <class Multiplexer>
class WebServ;

enum e_server_directive {
    SERVER_NAME,
    LISTEN,
    CLIENT_MAX_BODY_SIZE,
    ERROR_PAGE,
    RETURN,
    LOCATION,
    CGI
};

enum e_location_directive {};

typedef struct s_directive {
    std::string name;
    std::string argument;
}t_directive;

typedef struct s_server {
    uint32_t                    mask;
    std::vector<t_directive>    directiveList;
}t_server;

typedef struct s_webservConfig {
    std::vector<t_server>   serverList;
}t_webservConfig;

class Config {
public:
    const char* getConfigFilePath(void) const;

    void printServerConfig(void);
    void getServerLocationContext(std::ifstream& file, std::string& line,
        t_server& server, uint8_t directiveNbr);
    void getServerDirective(std::string& line,
        t_server& server, uint8_t directiveNbr);
    void getServerContext(std::ifstream& file, std::string& line);
    void parseConfigFile(void);

    Config(const char* configFilePath);
    ~Config();

private:
    std::string     _configFilePath;
    t_webservConfig _webservConfig;
};
