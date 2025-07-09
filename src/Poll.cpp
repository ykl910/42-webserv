#include "../include/Poll.hpp"
#include "../include/WebServ.hpp"

void    Poll::run(WebServ& server) {
    server.printServerStatus("poll");
}

Poll::Poll() {

}

Poll::~Poll() {

}
