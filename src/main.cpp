#include "../include/ServerSocket.hpp"

bool    got_config_file(int argc, char *argv) {
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

bool    choosed_multiplexer(char *input) {
    (void)input;
        return false;
    return true;
}

int main(int argc, char **argv) {
    if (got_config_file(argc, argv[1])) {
        try {
            // if (choosed_multiplexer()) // choose between select | poll | epoll
            //     ServerSocket server(argv[2], argv[3]);
            // else
            //     ServerSocket server(argv[2]);
            ServerSocket server;

            server.bindAndListen();
            std::cout << BOLD WHITE << "Server listening on port: "
            << BOLD BLUE << "8080" << DEFAULT << std::endl;
            server.acceptClient();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
