#pragma once

#include "AError.hpp"
#include <fstream>
#include <string>
#include <map>

template <class Multiplexer>
class WebServ;

class Config {
public:
    void parseConfigFile(void);

    Config();
    Config(const std::string& configPath);
    ~Config();

private:
    std::string _configPath;
    std::map<std::string, std::string>  _http;
    std::map<std::string, std::string>  _config;
};