#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <netdb.h>
#include <cerrno>

void printError();
void printErrorAndThrow(std::string const &context);
void printGaiErrorAndThrow(std::string const &context, int &status);
void manageConfigError(const std::string& line, const std::string& expected,
    const std::string& context, ssize_t lineNbr);
