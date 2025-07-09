#pragma once

#include <string>

class Config {
public:

    Config();
    Config(const std::string& filename);
    ~Config();

private:
    std::string _filename;
    const std::string _defaultPath;
};