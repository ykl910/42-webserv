#include "../../include/Config.hpp"

// void    Config::printConfig(void) const {
//     for (configIterator it = _config.begin(); it != _config.end(); ++it) {
//         t_server server = *it;
//         std::cout
//         << BOLD WHITE << "Server:\n" << DEFAULT
//         << "    " << server.port << "\n"
//         << "    " << server.host << "\n"
//         << "    " << server.server_name << "\n"
//         << "    " << server.client_max_body_size << "\n";
//     }
// }

void    Config::printServer(const server& srv) const {
    std::cout << "server:\n";
    for (contextIterator it = srv.begin(); it != srv.end(); ++it) {
        for (directiveIterator it2 = it->second.begin();
                             it2 != it->second.end(); ++it2) {
            std::cout << it2->second << "\n";
        }
    }
}

void    Config::printConfigParser(void) const {
    std::cout << "\n----- Print parser config -----\n";
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
                if (i == 0)
                    std::cout << "    " << it2->second << "\n";
                else
                    std::cout << "        " << it2->second << "\n";
        }
        ++i;
    }
    std::cout << "\n";
}
