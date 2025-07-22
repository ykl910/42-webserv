#include "../include/WebServ.hpp"

template <class Multiplexer>
void WebServ<Multiplexer>::printServerStatus(const char* multiplexer,
                                             const char* configFilePath) const {
    std::cout << BOLD WHITE << "Server status: "
    << BOLD ITALIC GREEN << "running\n"
    << BOLD WHITE << "Config: "
    << BOLD BLUE << configFilePath << "\n"
    << BOLD WHITE << "Port: "
    << BOLD ITALIC BLUE  << "8080\n"
    << BOLD WHITE << "Multiplexer: "
    << BOLD ITALIC BLUE  << multiplexer
    << DEFAULT << std::endl;
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ(const char* configFilePath)
    : _signalHandler(), _multiplexer(configFilePath) {
    runMultiplexer();
}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
