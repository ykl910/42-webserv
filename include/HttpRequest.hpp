#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <string>
#include <cstdio>
#include <vector>
#include <map>

class HttpRequest {
public:
    const std::string &getPath() const;
    const std::string &getBody() const;
    const std::string &getMethod() const;
    const std::string &getHttpVersion() const;
    const std::map<std::string, std::string> &getHeaders() const;
    void parse(const std::string &request);

    HttpRequest() {};
    HttpRequest(const std::string &request);

private:
    std::string                         _path;
    std::string                         _body;
    std::string                         _buffer;
    std::string                         _method;
    std::string                         _http_version;
    std::map<std::string, std::string>  _headers;
};
