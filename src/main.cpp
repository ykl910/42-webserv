#include "../include/ServerSocket.hpp"
#include "../include/multiplexer.hpp"

bool    got_config_file(const int argc, const char *argv) {
    if (argc == 2) {
        try {
            std::string input_file(argv);

            if (input_file.empty())
                throw std::runtime_error("Error: no config file name.");
            if (access(input_file.c_str(), R_OK) != 0)
                throw std::runtime_error("Error: can't read config file.");
        } catch (std::exception& e) {
            std::cerr << BOLD RED << e.what() << DEFAULT << std::endl;
            return false;
        }
        return true;
    }
    std::cout << BOLD WHITE << "Usage: ./webserv [config file]" << std::endl;
    return false;
}

bool    had_choosen_multiplexer(const char *input) {
    std::string multiplexer(input);

    return (multiplexer.empty() || multiplexer != "select"
        || multiplexer != "poll" || multiplexer != "epoll");
}

void    launch_specific_multiplexer(const char *config_file,
                                    const char *multiplexer) {
    std::string string_multiplexer(multiplexer);

    if (string_multiplexer == "select")
        run_with_select(config_file);
    else if (string_multiplexer == "poll")
        run_with_poll(config_file);
    else if (string_multiplexer == "epoll")
        run_with_epoll(config_file);

    ServerSocket server(config_file, multiplexer);
    server.bindAndListen();
    std::cout << BOLD WHITE << "Server listening on port: "
    << BOLD BLUE << "8080" << DEFAULT << std::endl;
    server.acceptClient();
}

int main(int argc, char **argv) {
    if (got_config_file(argc, argv[1])) {
        try {
            // choose between select | poll | epoll
            if (had_choosen_multiplexer(argv[3]))
                launch_specific_multiplexer(argv[2], argv[3]);
            else {
                //  ServerSocket server(argv[2]);
                ServerSocket server;

                server.bindAndListen();
                std::cout << BOLD WHITE << "Server listening on port: "
                << BOLD BLUE << "8080" << DEFAULT << std::endl;
                server.acceptClient();
            }
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
