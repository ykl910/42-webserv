#pragma once

#include "HttpRequest.hpp"
#include "CGI.hpp"
#include "Server.hpp"
#include "textFormatting.hpp"
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
#include <climits>

#define OFF 0
#define ON 1

#define GET 0
#define POST 1
#define DELETE 2

typedef struct s_request_attr t_request_attr;
// typedef struct s_serv_attr t_serv_attr;

class HttpManager;

class HttpResponse {
public:
    std::string getResponse();
    std::string& getResponseHead();
    std::string getStatusLine() const;
    std::string getRoot() const;
    t_serv_attr getServerAttr() const;
    t_request_attr getRequestAttr() const;

    const std::string getFileExtention(const std::string &requestPath) const;

    void setBody(const std::string &body);
    void setHeaders(const std::string &key, const std::string &value);
    void setStatusLine(const std::string version, int code, const std::string &reason);
    void buildResponse(int code, std::string msg);
    void handleError(std::stringstream *buffer, int success, std::string type);

    void solvePath();

    void handleGET();
    void handlePOST(HttpRequest& request);
    void handleDELETE();

    int handleImage();
    int handleRedirect();
    int handleCss(std::stringstream *buffer);
    int handleHtml(std::stringstream *buffer);

    void buildIndex();

    bool isCgi();
    bool isImage();
    bool isDirectory();
    bool isAutoIndex();

    std::string getBoundary(std::string &line);
    int createDirectory(std::string dirPath);
    bool directoryExist(std::string dirPath);
    int downloadFiles(HttpRequest &request, std::string boundary);
    int createFile(std::string &content, std::string &dirPath, int id);
    int createFd(std::string dir, std::string filename);
    bool canAccessFile(std::string& file);
    inline bool isFormData(std::string &contentType) const;
    bool isValidBodySize(HttpRequest &request, t_serv_attr &serverAttr) const;

    HttpResponse() {}
    HttpResponse(HttpRequest& request, t_serv_attr &serverAttr);
    ~HttpResponse() {}

private:
    std::string     _body;
    bool            _isAutoIndex;
    std::string     _statusLine;
    std::string     _responseHead;
    std::string     _extension;
    std::string     _response;
    headerMap       _headers;
    t_serv_attr     _server;
    t_request_attr  _request;
    uint8_t         _allowedMethod;
    std::string     _root;
    bool            _isIndex;
};

std::ostream& operator<<(std::ostream& os, HttpResponse& response);
