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

#define FAILURE 0
#define SUCCESS 1

class HttpRequest {
public:
    const bool &getState() const;
    const std::string &getPath() const;
    const std::string &getBody() const;
    const std::string &getMethod() const;
    const std::string &getContent() const;
    const std::string &getHttpVersion() const;
    const std::map<std::string, std::string> &getHeaders() const;

    void parseRequest(void);
    void readRequest(int clientFd);
    bool receivedCompleteRequest(std::string &rawData) const;

    HttpRequest() {}
    HttpRequest(int clientFd);
    ~HttpRequest() {}

private:
    bool                                _state;
    char                                _buffer[4096];
    std::string                         _path;
    std::string                         _body;
    std::string                         _method;
    std::string                         _content;
    std::string                         _http_version;
    std::map<std::string, std::string>  _headers;
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& request);
