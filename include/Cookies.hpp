#pragma once
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "utils.hpp"
#include "Error.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <string>
#include <vector>
#include <ctime>

class HttpResponse;
class HttpRequest;

#define POST42dotNET "./www/post42.net/"
#define WEBSERVdotNET "./www/webserv.net/"
// #define POST42dotNET WEBSERVdotNET

class Cookies {
public:
    Cookies(HttpRequest &request, HttpResponse &response, std::string boundary);
    ~Cookies();

private:
    void loadUserInfo();
    void buildCookieResponse(HttpRequest& request, HttpResponse& response, int code, std::string msg);
    int parseUsernamePwd(std::string username, std::string password);
    int registerUserInfo(HttpRequest &request, std::string boundary);
    int checkUsernamePwd(std::string username, std::string password);
    int authUserInfo(HttpRequest &request, std::string boundary);
    std::vector<std::map<std::string, std::string> > _userInfo;
    std::string generateSessionID();

};