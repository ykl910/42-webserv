#include "../../include/WebServ.hpp"

void printError()
{
    std::cerr << BOLD RED << "Error: " << DEFAULT;
    std::cerr << strerror(errno) << std::endl;
}

void manageConfigError(const std::string& line, const std::string& expected,
                        const std::string& context, ssize_t lineNbr)
{
    std::cerr
    << BOLD RED << "Error: " << DEFAULT << context << "\n";
    if (!expected.empty()) {
        std::cerr
        << BOLD WHITE << "line "
        << BOLD YELLOW << lineNbr
        << BOLD WHITE << " got:\n" << DEFAULT << line << "\n"
        << BOLD WHITE << "expected:\n" <<  DEFAULT << expected << "\n";
    }
    throw std::runtime_error("");
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
