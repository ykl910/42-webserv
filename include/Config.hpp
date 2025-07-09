#pragma once

#include "Error.hpp"
#include <string>

class WebServ;

class Config : public Error {
public:

    Config();
    Config(const std::string& filename);
    ~Config();

private:
    std::string _filename;
    const std::string _defaultPath;
};