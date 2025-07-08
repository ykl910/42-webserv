#include "../../include/WebServ.hpp"

void    run_using_select(WebServ& server) {
    server.bindAndListen();
    server.printServerStatus("select");
    server.multiplexSelect();
}
