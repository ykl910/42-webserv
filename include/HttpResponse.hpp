#pragma once

#include "HttpRequest.hpp"
#include "CGI.hpp"
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <map>

class HttpResponse {
public:
    HttpResponse(HttpRequest &request);
    HttpResponse() {};
    ~HttpResponse() {};
    void setStatusLine(const std::string version, int code, const std::string &reason);
    void setHeaders(const std::string &key, const std::string &value);
    void setBody(const std::string &body);
    std::string getResponse() const;
    std::string getStatusLine() const;
    void    build(HttpRequest &request);

private:
    std::string _statusLine;
    std::map<std::string, std::string> _headers;
    std::string _body;
};
