#include "../../include/ServerSocket.hpp"

void    run_using_epoll(ServerSocket& server) {
    server.bindAndListen();
    server.printServerStatus("epoll");
    server.acceptClientEpoll();
}