#include "../include/Config.hpp"

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


int getClientMaxBodySize(const std::string& input) {
    // for (size_t i = 0; input[i]; ++i) {

    // }
    return std::atoi(input.substr(0, input.length() - 1).c_str());
}

void    storeErrorPage(void) {
    return;
}

void    storeRedirection(void) {
    return;
}

void    storeLocation(void) {
    return;
}

void    storeCgi(void) {
    return;
}

void    Config::storeConfig(void)
{
    t_server    serverStruct;

    for (configParserIterator it = _configParser.begin();
                              it != _configParser.end(); ++it) {
        server indexServer = *it;

        serverStruct.port = indexServer[SERVER][LISTEN];
        serverStruct.host = indexServer[SERVER][HOST];
        serverStruct.server_name = indexServer[SERVER][SERVER_NAME];
        serverStruct.client_max_body_size =
        getClientMaxBodySize(indexServer[SERVER][CLIENT_MAX_BODY_SIZE]);
        // storeErrorPage();
        // storeRedirection();
        // storeLocation();
        // storeCgi();
        _config.push_back(serverStruct);
    }
}

Config::Config(const char* configFilePath)
    : _configFilePath(configFilePath)
{
    initConfigParser();
    // printConfigFormat();
    parseConfigFile();
    // printConfigParser();
    storeConfig();
    printConfig();
}

Config::~Config() {}
