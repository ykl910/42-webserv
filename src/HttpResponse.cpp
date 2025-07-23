#include "../include/HttpResponse.hpp"
#include "../include/GET.hpp"
#include "../include/POST.hpp"
#include "../include/DELETE.hpp"

void HttpResponse::build(HttpRequest &request){

    if (request.getMethod() == "GET") {
        handleGet(request, *this);
    }
    else if (request.getMethod() == "POST") {
        handlePost(request, *this);
    }
    else if (request.getMethod() == "DELETE") {
        handleDelete(request, *this);
    }
    else {

        //TODO : response with code 501
    }
}

void HttpResponse::setStatusLine(const std::string version, int code, const std::string &reason){
    std::ostringstream oss;
    oss << code;
    _statusLine = version + " " + oss.str() + " " + reason + "\r\n";
}

void HttpResponse::setHeaders(const std::string &key, const std::string &value){
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string &body){
    _body = body;
}

std::string HttpResponse::getResponse() const {
    std::string fullResponse;
    fullResponse += _statusLine;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        fullResponse += it->first + ": " + it->second + "\r\n";
    }
    fullResponse += "\r\n";
    fullResponse += _body;
    return fullResponse;
}

std::string HttpResponse::getStatusLine() const {
    return _statusLine;
}

HttpResponse::HttpResponse(HttpRequest &request){
    this->build(request);
}
