#include "../../include/Config.hpp"

void    Config::printServer(const server& srv) const
{
    std::cout << "server:\n";
    for (contextIterator it = srv.begin(); it != srv.end(); ++it) {
        for (directiveIterator it2 = it->second.begin();
                             it2 != it->second.end(); ++it2) {
            for (directiveValueIterator it3 = it2->second.begin();
                                        it3 != it2->second.end(); ++it3)
                std::cout << *it3 << "\n";
        }
    }
}

void    Config::printConfig(void) const
{
    std::cout << "\n----- Print config -----\n";
    for (configParserIterator it = _configParser.begin();
                        it != _configParser.end(); ++it) {
        printServer(*it);
    }
    std::cout << "\n";
}

void    Config::printConfigFormat(void) const
{
    const char* contextNameList[5] = {
        "server:", "error_page:", "redirection:", "location:", "cgi:"};

    std::cout
    << BOLD ITALIC BLUE << "[ WEBSERV CONFIGURATION FILE FORMAT ]\n"
    << BOLD WHITE << contextNameList[0] << DEFAULT << std::endl;

    size_t i = 0;
    for (configFormatIterator it = _configFormat.begin();
                              it != _configFormat.end(); ++it) {
        if (i != 0)
            std::cout << "    "
            << BOLD WHITE << contextNameList[i] << DEFAULT << "\n";
        for (directiveIterator it2 = it->second.begin();
                             it2 != it->second.end(); ++it2) {
                for (directiveValueIterator it3 = it2->second.begin();
                                           it3 != it2->second.end(); ++it3) {
                    if (i == 0)
                        std::cout << "    " << *it3 << "\n";
                    else
                        std::cout << "        " << *it3 << "\n";
                }
        }
        ++i;
    }
    std::cout << "\n";
}
