#pragma once

#include "Error.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>

template <class Multiplexer>
class WebServ;

enum e_context {MAIN_CONTEXT, HTTP_CONTEXT, SERVER_CONTEXT};

class Config {
public:
    void getNextContext(const std::string& context);
    void getNextDirective(const std::string& line);
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
    std::vector<int> _port;
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