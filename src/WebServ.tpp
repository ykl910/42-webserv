#include "../include/WebServ.hpp"

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

    config config = _config.getConfig();
    for (configIterator it = config.begin();
                        it != config.end(); ++it) {
        t_server server = *it;

        std::cout
        << BOLD WHITE << "Listen: "
        << BOLD ITALIC BLUE  << server.port << "\n" << DEFAULT;
    }
}


template <class Multiplexer>
WebServ<Multiplexer>::WebServ(const char* configFilePath,
                              const char* multiplexer)
    : _config(configFilePath), _multiplexer()
{
    printServerStatus(multiplexer, configFilePath);
    _multiplexer.run();
}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
