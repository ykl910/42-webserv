#include "../../include/WebServ.hpp"

void    run_using_epoll(WebServ& server) {
    server.bindAndListen();
    server.printServerStatus("epoll");
    server.multiplexEpoll();
}
