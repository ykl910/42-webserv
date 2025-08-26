#pragma once

#include <sstream>
#include <fcntl.h>
#include <string>
#include <vector>

std::string itos(int value);
std::vector<std::string>split(std::string string, std::string &boundary);