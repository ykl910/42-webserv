#include "../../include/WebServ.hpp"

void printError()
{
    std::cerr << BOLD RED << "Error: " << DEFAULT;
    std::cerr << strerror(errno) << std::endl;
}

void printErrorAndThrow(std::string const &context)
{
    std::ostringstream oss;
    oss << BOLD RED << "Error: " << DEFAULT;
    oss << context << "() failed: " << strerror(errno);
    throw std::runtime_error(oss.str());
}

void printGaiErrorAndThrow(std::string const &context, int &status)
{
    std::ostringstream oss;
    oss << BOLD RED << "Error: " << DEFAULT;
    oss << context << "() failed: " << gai_strerror(status);
    throw std::runtime_error(oss.str());
}
