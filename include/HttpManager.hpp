#pragma once

#include "textFormatting.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include <iostream>

typedef struct s_serv_attr t_serv_attr;

class HttpManager {
public:
    void sendResponse(int clientFd);
    void writeUserInfo(HttpRequest &request, HttpResponse &response);

    HttpManager() {}
    HttpManager(int clientFd, t_serv_attr &serverAttr);
    ~HttpManager();

private:
    HttpRequest     _request;
    HttpResponse    _response;
};
