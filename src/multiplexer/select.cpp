#include "../../include/WebServ.hpp"

void    run_using_select(WebServ& server) {
    server.initServer();
    server.printServerStatus("select");
    server.multiplexSelect();
}
