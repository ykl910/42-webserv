#include "../include/WebServ.hpp"
#include "../include/HttpRequest.hpp"

bool    got_config_file_path(const int argc, const char* argv) {
    if (argc == 2 || argc == 3) {
        try {
            std::string config_file(argv);
            if (config_file.empty())
                throw std::runtime_error("Error: no config file name.");
            if (access(config_file.c_str(), R_OK) != 0)
                throw std::runtime_error("Error: can't read config file.");
            return true;
        } catch (std::exception& e) {
            std::cerr << BOLD RED << e.what() << DEFAULT << std::endl;
        }
    }
    return false;
}

bool    had_choosen_multiplexer(const std::string& input) {
    return !input.empty() && (input == "select"
        || input == "poll" || input == "epoll");
}

void    run_specific_multiplexer(const std::string& multiplexer,
                                 const char* configFilePath) {
    if (multiplexer == "select")
        WebServ<Select> server(configFilePath);
    else if (multiplexer == "poll")
        WebServ<Poll>   server(configFilePath);
    else if (multiplexer == "epoll")
        WebServ<Epoll>  server(configFilePath);
}

int main(int argc, char **argv) {
    std::string configFilePath("config/simple.conf");

    if (got_config_file_path(argc, argv[1]))
        configFilePath = std::string(argv[1]);
    try {
        if ((argc == 2 || argc == 3) && had_choosen_multiplexer(argv[argc - 1]))
            run_specific_multiplexer(argv[2], configFilePath.c_str());
        else
            WebServ<Select> server(configFilePath.c_str());
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//test response parsing
// int main() {
//     std::string raw_request =
//         "GET /index.html HTTP/1.1\r\n"
//         "Host: localhost:8080\r\n"
//         "User-Agent: curl/7.64.1\r\n"
//         "Accept: */*\r\n"
//         "\r\n";

//     HttpRequest request(raw_request);
//     // std::cout << "Method: " << request.getMethod() << std::endl;
//     // std::cout << "Path: " << request.getPath() << std::endl;
//     // std::cout << "Version: " << request.getHttpVersion() << std::endl;
//     // std::map<std::string, std::string> m = request.getHeaders();
//     // for (std::map<std::string, std::string>::iterator it = m.begin();
//     //     it != m.end(); ++it) {
//     //     std::cout << it->first << "-> " << it->second << std::endl;
//     // }
//     HttpResponse response(request);
//     response.build(request);
//     std::cout << response.getResponse();
// }
