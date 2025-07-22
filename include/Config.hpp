#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>

template <class Multiplexer>
class WebServ;

enum e_directive_list {SERVER, ERROR_PAGE, RETURN, LOCATION, CGI};

enum e_server_directive {
    SERVER_NAME,
    LISTEN,
    CLIENT_MAX_BODY_SIZE
};

enum e_error_page_directive {
    E_404,
    E_
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
    typedef std::map<uint8_t, std::map<uint8_t, std::string> > directive;

    const char* getConfigFilePath(void) const;

    void initConfig(directive& directiveList);
    void printServerConfig(void);
    void getServerLocationContext(std::ifstream& file, std::string& line,
        t_server& server, uint8_t directiveNbr);
    void getServerDirective(std::string& line,
        t_server& server, uint8_t directiveNbr);
    void getServerContext(std::ifstream& file, std::string& line);
    void parseConfigFile(directive& directiveList);

    Config(const char* configFilePath);
    ~Config();

private:
    std::string     _configFilePath;
    t_webservConfig _webservConfig;

};
