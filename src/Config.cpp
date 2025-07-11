#include "../include/Config.hpp"
#include "../include/WebServ.hpp"

// parseConfigFile(this->_configFile);

void    Config::parseConfigFile(void) {
    std::ifstream file(this->_configPath.c_str());

    if (!file)
        throw std::runtime_error("Error: can't open config file");
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
}

Config::Config() : _configPath("config/webserv.conf") {
    // Parse config file with default path
    this->parseConfigFile();
}

Config::Config(const std::string& configPath) : _configPath(configPath) {
    this->parseConfigFile();
    // Parse config file
}

Config::~Config() {

}