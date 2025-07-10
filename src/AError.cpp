#include "../include/WebServ.hpp"

void AError::printError() const {

    std::cerr << BOLD RED << "Error: " << DEFAULT;
    std::cerr << strerror(errno) << std::endl;
}

void AError::printErrorAndThrow(std::string const &context) const {
        std::ostringstream oss;
        oss << BOLD RED << "Error: " << DEFAULT;
        oss << context << "() failed: " << strerror(errno);
        throw std::runtime_error(oss.str());
}

void AError::printGaiErrorAndThrow(std::string const &context, int &status) const {
        std::ostringstream oss;
        oss << BOLD RED << "Error: " << DEFAULT;
        oss << context << "() failed: " << gai_strerror(status);
        throw std::runtime_error(oss.str());
}
