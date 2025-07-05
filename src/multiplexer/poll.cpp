#include "../../include/ServerSocket.hpp"

void    run_using_poll(ServerSocket& server) {
    server.bindAndListen();
    server.printServerStatus("poll");
    server.acceptClientPoll();
}
