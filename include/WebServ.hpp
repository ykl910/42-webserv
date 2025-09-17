#pragma once

#include "textFormatting.hpp"
#include "Select.hpp"
#include "Config.hpp"
#include "Error.hpp"
#include "Epoll.hpp"
#include "Poll.hpp"

typedef std::vector<Server>::iterator serverIterator;

template <class Multiplexer>
class WebServ {
public:
    void printServerStatus(const char* multiplexer, const char* configFilePath);

    WebServ(Config& config, const char* multiplexer);
    ~WebServ();

private:
    Multiplexer _multiplexer;
};
#include "../src/server/WebServ.tpp"
