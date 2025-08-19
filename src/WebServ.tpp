#include "../include/WebServ.hpp"
#include "../include/Config.hpp"

template <class Multiplexer>
void WebServ<Multiplexer>::printServerStatus(const char* multiplexer,
                                             const char* configFilePath) const
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

int getClientMaxBodySize(const std::string& input) {
    // for (size_t i = 0; input[i]; ++i) {

    // }
    return std::atoi(input.substr(0, input.length() - 1).c_str());
}



template <class Multiplexer>
void    WebServ<Multiplexer>::createServer(void)
{
    for (configParserIterator it = _config._configParser.begin();
                              it != _config._configParser.end(); ++it) {
        server      configServer = *it;
        t_server    newServer;

        newServer.port = configServer[SERVER][LISTEN];
        newServer.host = configServer[SERVER][HOST];
        newServer.server_name = configServer[SERVER][SERVER_NAME];
        newServer.client_max_body_size =
        getClientMaxBodySize(configServer[SERVER][CLIENT_MAX_BODY_SIZE]);
        // storeErrorPage();
        // storeRedirection();
        // storeLocation();
        // storeCgi();
        newServer.socket = Socket(newServer.port.c_str(), newServer.host.c_str());
        _server.push_back(newServer);
    }
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ(const char* configFilePath,
                              const char* multiplexer)
    : _config(configFilePath), _multiplexer()
{
    createServer();
    printServerStatus(multiplexer, configFilePath);
    _multiplexer.run(_server);
}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
