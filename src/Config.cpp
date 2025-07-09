#include "../include/Config.hpp"

Config::Config() : _defaultPath("config/webserv.conf") {

}

Config::Config(const std::string& filename) : _filename(filename) {

}

Config::~Config() {

}