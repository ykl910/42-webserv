#pragma once

#include "HttpRequest.hpp"
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
    void setStatusLine(int code, std::string &reason);
    void setHeaders(std::string &key, std::string &value);
    void setBody(std::string &body);
    std::string getResponse() const;
    void    build(HttpRequest &request);

private:
    std::string _statusLine;
    std::map<std::string, std::string> _headers;
    std::string _body;
};
