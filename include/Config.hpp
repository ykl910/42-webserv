#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <vector>
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

    typedef std::map<std::string,
            std::map<std::string,
            std::vector<std::string> > > context;
    context _mainContext;
    context _httpContext;
    context _serverContext;
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