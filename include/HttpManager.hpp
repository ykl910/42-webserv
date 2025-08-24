#pragma once

#include "textFormatting.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <iostream>

class HttpManager {
public:
    void sendResponse(int clientFd);

    HttpManager(int clientFd, t_serv_attr &serverAttr);
    ~HttpManager();

private:
    HttpRequest     _request;
    HttpResponse    _response;
};
