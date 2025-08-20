#include "../include/WebServ.hpp"

template <class Multiplexer>
void WebServ<Multiplexer>::printServerStatus(const char* multiplexer,
                                             const char* configFilePath)
{
    std::cout << BOLD WHITE << "Program status: "
    << BOLD ITALIC GREEN << "running\n" << DEFAULT
    << BOLD WHITE << "Config: "
    << BOLD ITALIC BLUE << configFilePath << "\n" << DEFAULT
    << BOLD WHITE << "Multiplexer: "
    << BOLD ITALIC BLUE  << multiplexer << "\n" << DEFAULT;

    for (configIterator it = _server.begin();
                        it != _server.end(); ++it) {
        t_server server = *it;
        std::cout
        << BOLD WHITE << "Listen: "
        << BOLD ITALIC BLUE  << server.port << "\n" << DEFAULT;
    }
}

static int getClientMaxBodySize(const std::string& input) {
    // for (size_t i = 0; input[i]; ++i) {

    // }
    return std::atoi(input.substr(0, input.length() - 1).c_str());
}

static void    storeErrorPage(void) {
    return;
}

static void    storeRedirection(void) {
    return;
}

static void    storeLocation(void) {
    return;
}

static void    storeCgi(void) {
    return;
}

template <class Multiplexer>
void    WebServ<Multiplexer>::createServer(void)
{
    configParser parser = _config.getConfigParser();
    int i = 0;
    for (configParserIterator it = parser.begin();
                              it != parser.end(); ++it) {
        server      configServer = *it;
        t_server    newServer;

        newServer.port = configServer[SERVER][LISTEN];
        newServer.host = configServer[SERVER][HOST];
        newServer.server_name = configServer[SERVER][SERVER_NAME];
        newServer.client_max_body_size =
        getClientMaxBodySize(configServer[SERVER][CLIENT_MAX_BODY_SIZE]);
        storeErrorPage();
        storeRedirection();
        storeLocation();
        storeCgi();
        newServer.socket = Socket(newServer.port.c_str(), newServer.host.c_str());
        _server.push_back(newServer);
        ++i;
    }
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ(const char* configFilePath,
                              const char* multiplexer)
    : _config(configFilePath)
{
    createServer();
    _multiplexer = Multiplexer(_server);
    printServerStatus(multiplexer, configFilePath);
    _multiplexer.run();
}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
