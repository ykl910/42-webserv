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

typedef struct s_request_attr t_request_attr;

class HttpResponse {
public:
    std::string getResponse();
    std::string& getResponseHeader();
    std::string getStatusLine() const;
    std::string getRoot() const;
    t_serv_attr getServerAttr() const;
    t_request_attr getRequestAttr() const;

    const std::string getFileExtention(const std::string &requestPath) const;

    void setBody(const std::string &body);
    void setHeaders(const std::string &key, const std::string &value);
    void setStatusLine(const std::string version, int code, const std::string &reason);
    void buildResponse(HttpRequest& request, int code, std::string msg);
    void handleError(std::stringstream *buffer, int success, std::string type);
    void handleGET();
    void handlePOST(HttpRequest& request);
    void handleDELETE();
    void solveMimeType();
    void solvePath();

    void buildResponse(void);
    int handleImage();
    int handleRedirect();
    int handleCss(std::stringstream *buffer);
    int handleHtml(std::stringstream *buffer);

    bool isValidBodySize(HttpRequest &request, t_serv_attr &serverAttr) const;
    bool isAudio();
    bool isImage();
    bool isCgi(std::string &Requestpath);
    bool canAccessFile(std::string& file);
    inline bool isFormData(std::string &contentType) const;

    HttpResponse() {}
    HttpResponse(HttpRequest& request, t_serv_attr &serverAttr);
    ~HttpResponse() {}

private:
    std::string     _body;
    std::string     _statusLine;
    std::string     _response;
    std::string     _extension;
    headerMap       _headers;
    t_serv_attr     _server;
    t_request_attr  _request;
    uint8_t         _allowedMethod;
    std::string     _root;

};

std::ostream& operator<<(std::ostream& os, HttpResponse& response);
