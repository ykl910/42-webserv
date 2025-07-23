#pragma once
#include "Error.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "utils.hpp"
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <fstream>
#include <ctime>


class HttpResponse;
class HttpRequest;

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