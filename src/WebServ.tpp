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

    std::vector<Server*> server = _multiplexer.getServer();
    for (serverIterator it = server.begin();
                        it != server.end(); ++it) {
        t_serv_attr attribute = (*it)->getServerAttribute();
        std::cout
        << BOLD WHITE << "Listen: "
        << BOLD ITALIC BLUE  << attribute.port << "\n" << DEFAULT;
    }
}

// template <class Multiplexer>
// void    WebServ<Multiplexer>::createServer(Config& config)
// {
//     configParser parser = config.getConfigParser();
//     for (configParserIterator it = parser.begin();
//                               it != parser.end(); ++it) {
//         server config = *it;
//         Server newServer(config);
//         _server.push_back(newServer);
//     }
// }

template <class Multiplexer>
WebServ<Multiplexer>::WebServ(Config& config, const char* multiplexer)
    : _multiplexer(config)
{
    printServerStatus(multiplexer, config.getConfigFilePath());
    _multiplexer.run();
}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
