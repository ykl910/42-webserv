#include "../../include/utils.hpp"

std::string itos(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::vector<std::string>split(std::string string, std::string &boundary)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end;

    while ((end = string.find(boundary, start)) != std::string::npos) {
        tokens.push_back(string.substr(start, end - start));
        start = end + boundary.length();
    }
    tokens.push_back(string.substr(start));
    return tokens;
}
