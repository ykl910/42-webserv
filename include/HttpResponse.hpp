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
    ~HttpResponse();
    void setCode(int code);
    void setHeaders(std::string &key, std::string &value);
    void setBody(std::string &body);
    std::string getResponse() const;

private:
    void    build();
    std::string statusLine;
    std::map<std::string, std::string> headers;
    std::string body;
};
