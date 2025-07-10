#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <netdb.h>
#include <cerrno>

class AError {
public:
    void printError() const;
    void printErrorAndThrow(std::string const &context) const;
    void printGaiErrorAndThrow(std::string const &context, int &status) const;

private:
    // AError() {};
    // ~AError() {};
};
