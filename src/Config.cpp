#include "../include/Config.hpp"
#include "../include/WebServ.hpp"

// parseConfigFile(this->_configFile);

Config::Config() : _defaultPath("config/webserv.conf") {
    // Parse config file with default path
}

Config::Config(const std::string& filename) : _filename(filename) {
    // Parse config file
}

Config::~Config() {

}