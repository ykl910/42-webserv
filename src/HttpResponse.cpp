#include "../include/HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest &request){
    (void)request;
}

HttpResponse::~HttpResponse(){}

void HttpResponse::setCode(int code){
    (void)code;
}

void HttpResponse::setHeaders(std::string &key, std::string &value){
    (void)key;
    (void)value;
}

void HttpResponse::setBody(std::string &body){
    (void)body;
}

std::string HttpResponse::getResponse() const {

    return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello from webserv!</h1></body></html>";
}

void HttpResponse::build(){}
