#include "../../include/WebServ.hpp"

template <class Multiplexer>
void WebServ<Multiplexer>::printServerStatus(const char* multiplexer,
                                             const char* configFilePath)
{
    std::cout
    << BOLD WHITE << "Program status: "
    << BOLD ITALIC GREEN << "running\n" << DEFAULT
    << BOLD WHITE << "Config: "
    << BOLD ITALIC BLUE << configFilePath << "\n" << DEFAULT
    << BOLD WHITE << "Multiplexer: "
    << BOLD ITALIC BLUE  << multiplexer << "\n" << DEFAULT;

    std::vector<Server> server = _multiplexer.getServer();
    int i = 0;
    for (serverIterator it = server.begin();
                        it != server.end(); ++it) {
        t_serv_attr attribute = it->getServerAttribute();
        std::cout
        << BOLD WHITE << "Server "
        << BOLD CYAN << i << "\n"
        << BOLD WHITE << "    host: "
        << BOLD ITALIC BLUE  << attribute.host << "\n" << DEFAULT
        << BOLD WHITE << "    port: "
        << BOLD ITALIC BLUE  << attribute.port << "\n" << DEFAULT
        << BOLD WHITE << "    server_name: "
        << BOLD ITALIC BLUE  << attribute.server_name << "\n" << DEFAULT;
        ++i;
    }
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ(Config& config, const char* multiplexer)
    : _multiplexer(config)
{
    printServerStatus(multiplexer, config.getConfigFilePath());
    _multiplexer.run();
}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
