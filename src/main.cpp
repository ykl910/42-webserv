#include "../include/WebServ.hpp"
#include "../include/ServerSocket.hpp"

int main() {
    ServerSocket server;
    server.bindAndListen();
    std::cout << "Server listening on port 8080..." << std::endl;
    server.acceptClient();
    return 0;
}
