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
    void getRequest(int clientFd);
    bool receivedCompleteRequest(std::string &rawData) const;
    void sendResponse(int clientFd, HttpRequest& request, t_serv_attr & servAttr);
    void writeUserInfo(HttpRequest &request, HttpResponse &response);

    HttpManager() {}
    HttpManager(int clientFd, t_serv_attr &serverAttr);
    ~HttpManager();

private:
    static std::map<int, HttpRequest>   _request;
    static std::map<int, HttpResponse>  _responses;

    static std::map<int, std::string>   _buffers;
    static std::map<int, bool>          _gotResponse;
    static std::map<int, bool>          _gotFullRequest;
    static std::map<int, int>           _pendingResponse;
};
