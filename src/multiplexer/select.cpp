#include "../../include/ServerSocket.hpp"

void    run_using_select(ServerSocket& server) {
    server.bindAndListen();
    server.printServerStatus("select");
    server.acceptClientSelect();
}