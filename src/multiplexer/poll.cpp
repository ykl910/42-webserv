#include "../../include/ServerSocket.hpp"

void    run_using_poll(ServerSocket& server) {
    server.bindAndListen();
    std::cout << BOLD WHITE << "Server listening on port: "
    << BOLD BLUE << "8080" << DEFAULT << std::endl;
    server.acceptClient();
}
