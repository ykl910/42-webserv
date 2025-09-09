#pragma once

#include "textFormatting.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include <sys/epoll.h>
#include <iostream>

#define OUT 0
#define IN 1

// Request include an define
// #include <netinet/in.h>
// #include <unistd.h>
// #include <iostream>
// #include <cstdlib>
// #include <sstream>
// #include <cstring>
// #include <cerrno>
// #include <string>
// #include <cstdio>
// #include <vector>
// #include <map>

#define FAILURE 0
#define SUCCESS 1

typedef struct s_serv_attr t_serv_attr;

enum e_success_code {
    OK_200 = 200,
    CREATED_201,
    ACCEPTED_202
};

enum e_state {
    PENDING,
    RECEIVED,
    RESPONSE_TRUNCATE,
    SENT
};

class HttpManager {
public:
    void getRequest(int clientFd, t_serv_attr &serverAttr, int &clientState, bool &persistance);
    bool receivedCompleteRequest(std::string &rawData, t_serv_attr &serverAttr) const;
    void sendResponse(int clientFd, HttpRequest& request, t_serv_attr & servAttr, int &clientState);
    void writeUserInfo(HttpRequest &request, HttpResponse &response);
    static bool hasCompletedResponse(int clientFd);

    // Request methods
    const bool &getState() const;
    const std::string &getPath() const;
    const std::string &getBody() const;
    const std::string &getMethod() const;
    const std::string &getContent() const;
    const std::string &getHttpVersion() const;
    const t_request_attr& getRequestAttr() const;
    const std::map<std::string, std::string> &getHeaders() const;
    void extractRequest(const std::string &request);
    inline bool checkPersistance(std::string &request);


    HttpManager() {}
    HttpManager(int clientFd, t_serv_attr &serverAttr, int &clientState, bool &persistance);
    ~HttpManager();

private:
    static std::map<int, HttpRequest>   _request;
    static std::map<int, HttpResponse>  _responses;

    static std::map<int, std::string>   _buffers;
    static std::map<int, bool>          _gotResponse;
    static std::map<int, bool>          _gotFullRequest;
    static std::map<int, int>           _pendingResponse;
    static std::map<int, bool>          _responseSent;

    // Request attributes
    // bool                                _state;
    // std::string                         _path;
    // std::string                         _body;
    // std::string                         _method;
    // std::string                         _content;
    // std::string                         _http_version;
    // t_request_attr                      _requestAttr;
    // std::map<std::string, std::string>  _headers;
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& request);
