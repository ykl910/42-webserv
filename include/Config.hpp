#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <map>

template <class Multiplexer>
class WebServ;

class Config {
public:
    void parseConfigFile(void);

    Config();
    Config(const std::string& configPath);
    ~Config();

private:
    std::string _configPath;
    // std::map<std::string, std::map<std::string, std::map<std::string, std::string>  _mainContext;
    // std::map<std::string, std::map<std::string, std::map<std::string, std::string>  _mainContext;
    // std::map<std::string, std::map<std::string, std::map<std::string, std::string>  _httpContext;
    // std::map<std::string, std::map<std::string, std::map<std::string, std::string>  _config;
};

/* NGINX context hierarchy
main
├── events
├── http
│   ├── server
│   │   ├── location
│   │   │   ├── if
│   │   │   └── limit_except
│   ├── upstream
│   ├── map
│   └── geo
├── stream
│   └── server
└── mail
    └── server
*/