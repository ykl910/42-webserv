#include "../include/WebServ.hpp"

// Construction
WebServ::WebServ() {
    bzero(this, sizeof(WebServ));
}

WebServ::WebServ(t_config& config){
    bzero(this, sizeof(WebServ));
}

// Destruction
WebServ::~WebServ() {

}