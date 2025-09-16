#pragma once

#include "textFormatting.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "ServerTypes.hpp"
#include "utils.hpp"
#include <sys/epoll.h>
#include <iostream>

#define CONTENT_LENGTH_SIZE 16

#define OUT 0
#define IN 1

#define FAILURE 0
#define SUCCESS 1

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

class HttpResponse;

class HttpManager {
public:
    void getRequest(int clientFd, int &clientState, bool &persistance);
    bool receivedCompleteRequest(std::string &rawData) const;
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
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& request);
