#pragma once

#include "HttpRequest.hpp"
#include "Server.hpp"
#include "CGI.hpp"
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <string>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <map>

class HttpResponse {
public:
    std::string getResponse() const;
    std::string getStatusLine() const;
    t_serv_attr getServerAttr() const;
    void setBody(const std::string &body);
    void setHeaders(const std::string &key, const std::string &value);
    void setStatusLine(const std::string version, int code, const std::string &reason);

    HttpResponse(HttpRequest &request, t_serv_attr &serverAttr);
    HttpResponse() {};
    ~HttpResponse() {};

private:
    std::string                         _body;
    std::string                         _statusLine;
    std::string                         _response;
    std::map<std::string, std::string>  _headers;
    t_serv_attr                         _servAttr;
};

std::ostream& operator<<(std::ostream& os, const HttpResponse& response);