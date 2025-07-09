#include "../../include/WebServ.hpp"

void    run_using_poll(WebServ& server) {
    server.initServer();
    server.printServerStatus("poll");
    server.multiplexPoll();
}
