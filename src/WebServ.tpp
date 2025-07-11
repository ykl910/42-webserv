#include "../include/WebServ.hpp"

template <class Multiplexer>
void WebServ<Multiplexer>::printServerStatus(const char* multiplexer) const {
    std::cout << BOLD WHITE << "Server status: "
    << BOLD ITALIC GREEN << "running\n" << DEFAULT
    << BOLD WHITE << "Port: "
    << BOLD ITALIC BLUE  << "8080\n" << DEFAULT
    << BOLD WHITE << "Multiplexer: "
    << BOLD ITALIC BLUE  << multiplexer << DEFAULT
    << std::endl;
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ() : _signalHandler(), _multiplexer() {
    this->runMultiplexer();
}

template <class Multiplexer>
WebServ<Multiplexer>::WebServ(const char* configPath)
    : Multiplexer(configPath) {}

template <class Multiplexer>
WebServ<Multiplexer>::~WebServ() {}
