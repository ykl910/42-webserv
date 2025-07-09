#include "../include/WebServ.hpp"
#include "../include/HttpRequest.hpp"

bool    got_config_file(const int argc, const char* argv) {
    if (argc == 2 || argc == 3) {
        try {
            std::string config_file(argv);
            if (config_file.empty())
                throw std::runtime_error("Error: no config file name.");
            if (access(config_file.c_str(), R_OK) != 0)
                throw std::runtime_error("Error: can't read config file.");
        } catch (std::exception& e) {
            std::cerr << BOLD RED << e.what() << DEFAULT << std::endl;
            return false;
        }
        return true;
    }
    std::cout << BOLD WHITE << "Usage: ./webserv [config file]" << DEFAULT
    << std::endl;
    return false;
}

bool    had_choosen_multiplexer(const char* input) {
    std::string multiplexer(input);

    return !multiplexer.empty() && (multiplexer == "select"
        || multiplexer == "poll" || multiplexer == "epoll");
}

void    run_specific_multiplexer(WebServ& server,
                                 const std::string& multiplexer) {
    std::string string_multiplexer(multiplexer);

    if (string_multiplexer == "select")
        server.runSelect();
    else if (string_multiplexer == "poll")
        server.runPoll();
    else if (string_multiplexer == "epoll")
        server.runEpoll();
}

int main(int argc, char **argv) {
    if (got_config_file(argc, argv[1])) {
        try {
            WebServ    server(argv[1]);

            server.initServer();
            // choose between select | poll | epoll
            if (argc == 3 && had_choosen_multiplexer(argv[2]))
                run_specific_multiplexer(server, argv[2]);
            else
                server.runSelect();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

//test request parsing
// int main() {
//     std::string raw_request =
//         "GET /index.html HTTP/1.1\r\n"
//         "Host: localhost:8080\r\n"
//         "User-Agent: curl/7.64.1\r\n"
//         "Accept: */*\r\n"
//         "\r\n";

//     httpRequest request(raw_request);
//     std::cout << "Method: " << request.getMethod() << std::endl;
//     std::cout << "Path: " << request.getPath() << std::endl;
//     std::cout << "Version: " << request.getHttpVersion() << std::endl;
//     std::map<std::string, std::string> m = request.getHeaders();
//     for (std::map<std::string, std::string>::iterator it = m.begin();
//         it != m.end(); ++it) {
//         std::cout << it->first << "-> " << it->second << std::endl;
//     }
// }
