#include "../../include/WebServ.hpp"

void    run_using_poll(WebServ& server) {
    server.bindAndListen();
    server.printServerStatus("poll");
    server.acceptClientPoll();
}
