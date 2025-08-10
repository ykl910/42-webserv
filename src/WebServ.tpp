#include "../include/WebServ.hpp"

template <class Multiplexer>
void WebServ<Multiplexer>::printServerStatus(const char* multiplexer,
                                             const char* configFilePath) const
{
    std::cout << BOLD WHITE << "Server status: "
    << BOLD ITALIC GREEN << "running\n" << DEFAULT
    << BOLD WHITE << "Config: "
    << BOLD ITALIC BLUE << configFilePath << "\n" << DEFAULT
    << BOLD WHITE << "Port: "
    << BOLD ITALIC BLUE  << "8080\n" << DEFAULT
    << BOLD WHITE << "Multiplexer: "
    << BOLD ITALIC BLUE  << multiplexer << DEFAULT
    << DEFAULT << std::endl;
}

template <class Multiplexer>
void    WebServ<Multiplexer>::initSignalHandler(void)
{
    if (signal(SIGINT, sigHandler) == SIG_ERR
        || signal(SIGQUIT, sigHandler) == SIG_ERR)
        throw std::runtime_error("Error: signal init.");
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ(const char* configFilePath, const char* multiplexer)
    : _config(configFilePath), _multiplexer()
{
    initSignalHandler();
    printServerStatus(multiplexer, configFilePath);
    _multiplexer.run();
}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
