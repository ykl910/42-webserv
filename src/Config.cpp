#include "../include/Config.hpp"
#include "../include/WebServ.hpp"

Config::Config() : _defaultPath("config/webserv.conf") {

}

Config::Config(const std::string& filename) : _filename(filename) {
    // Parse config file

}

Config::~Config() {

}