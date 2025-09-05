#pragma once

#include "HttpRequest.hpp"
#include "CGI.hpp"
#include "Server.hpp"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <string>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <map>

typedef struct s_response_attr
{
    std::string method;
    std::string path;
    std::string host;
}t_response_attr;

class HttpResponse {
public:
    std::string getResponse();
    std::string& getResponseHeader();
    std::string getStatusLine() const;
    t_serv_attr getServerAttr() const;

    void setBody(const std::string &body);
    void setHeaders(const std::string &key, const std::string &value);
    void setStatusLine(const std::string version, int code, const std::string &reason);

    void resolveRequestHeaders(HttpRequest& request);
    void buildResponse(HttpRequest& request, int code, std::string msg);

    void handleThread(HttpRequest& request);
    int handleImg(HttpRequest& request, std::string path, std::string extension);
    int handleCss(HttpRequest& request, std::string path, std::stringstream *buffer);
    int handleHtml(HttpRequest& request, std::string path, std::stringstream *buffer);
    void handleError(HttpRequest& request, std::stringstream *buffer, int success, std::string type);

    void handleGET(HttpRequest& request);
    void handlePOST(HttpRequest& request);
    void handleDELETE(HttpRequest& request);

    HttpResponse() {}
    HttpResponse(HttpRequest &request, t_serv_attr &serverAttr);
    ~HttpResponse() {}

private:
    std::string                         _body;
    std::string                         _statusLine;
    std::string                         _response;
    std::map<std::string, std::string>  _headers;
    t_serv_attr                         _servAttr;
};

std::ostream& operator<<(std::ostream& os, HttpResponse& response);