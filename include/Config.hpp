#pragma once

#include "AError.hpp"
#include <string>

template <class Multiplexer>
class WebServ;

class Config : public AError {
public:

    Config();
    Config(const std::string& filename);
    ~Config();

private:
    std::string _filename;
    const std::string _defaultPath;
};