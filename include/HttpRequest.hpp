#pragma once

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

class HttpRequest {
public:
    HttpRequest() {};
    HttpRequest(const std::string &request);
    const std::string &getMethod() const;
    const std::string &getPath() const;
    const std::string &getHttpVersion() const;
    const std::map<std::string, std::string> &getHeaders() const;
    const std::string &getBody() const;

private:
    void    _parse(const std::string &request);
    std::string _method;
    std::string _path;
    std::string _http_version;
    std::map<std::string, std::string> _headers;
    std::string _body;
};
