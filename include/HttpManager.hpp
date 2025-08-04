#pragma once

#include "textFormatting.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <iostream>

class HttpManager {
public:
    HttpManager(int clientFd);
    ~HttpManager();

private:
    HttpRequest     _request;
    HttpResponse    _response;
    char            _buffer[4096];
    ssize_t         _bytes;
};
