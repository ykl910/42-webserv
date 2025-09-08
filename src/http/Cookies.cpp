#include "../../include/Cookies.hpp"

void Cookies::loadUserInfo()
{
    std::string fullpath = POST42dotNET"users/userinfo";
    std::ifstream file(fullpath.c_str());
    if(!file.is_open())
        return;

    std::string line;
    while(std::getline(file, line)) {
        size_t usernamePos = line.find("username:");
        size_t usernameEnd = line.find(";");
        size_t passwordPos = line.find("password:");

        if(usernamePos != std::string::npos && passwordPos != std::string::npos) {
            std::string username = line.substr(usernamePos + 9, usernameEnd - usernamePos - 9);
            std::string password = line.substr(passwordPos + 9);

            std::map<std::string, std::string> user;
            user["username"] = username;
            user["password"] = password;
            _userInfo.push_back(user);
        }
    }
    file.close();
}

// register
int Cookies::parseUsernamePwd(std::string username, std::string password)
{
    std::string fullpath = POST42dotNET"users/userinfo";
    int fd = open(fullpath.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
    if(fd == -1)
        return -1;
    size_t startPosUser = username.find("\r\n\r\n") + 4;
    size_t endPosUser = username.size() - 4;
    std::string user = username.substr(startPosUser, endPosUser - startPosUser);
    size_t startPosPwd = password.find("\r\n\r\n") + 4;
    size_t endPosPwd = password.size() - 4;
    std::string pwd = password.substr(startPosPwd, endPosPwd - startPosPwd);

    loadUserInfo();
    std::vector<std::map<std::string, std::string> >::iterator it;
    for(it = _userInfo.begin(); it != _userInfo.end(); ++it) {
        if((*it)["username"] == user)
            return -2;
    }
    std::map<std::string, std::string> m;
    m["username"] = user;
    m["password"] = pwd;
    _userInfo.push_back(m);
    std::string ent = "username:" + user + ";password:" + pwd + "\n";
    if(write(fd, ent.c_str(), ent.length()) == -1)
        return -1;
    close(fd);
    return 0;
}

int Cookies::registerUserInfo(HttpRequest &request, std::string boundary)
{
    std::vector<std::string> tokens = split(request.getBody(), boundary);
    std::vector<std::string>::iterator tokenIt;
    std::string userLine = "";
    std::string pwdLine = "";

    for(tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt)
    {
        std::string line = *tokenIt;
        if(line.find("Content-Disposition: form-data; name=\"username\"") != std::string::npos)
        {
            userLine = line;
            continue;
        }
        if(line.find("Content-Disposition: form-data; name=\"password\"")!= std::string::npos)
        {
            pwdLine = line;
        }
        if (userLine != "" && pwdLine != "")
        {
            int status = parseUsernamePwd(userLine, pwdLine);
            if(status == -1)
                return -1;
            else if(status == -2)
                return -2;
            break;
        }
    }
    if (userLine == "" || pwdLine == "")
        return -1;
    return 0;
}

// login
int Cookies::checkUsernamePwd(std::string username, std::string password)
{
    size_t startPosUser = username.find("\r\n\r\n") + 4;
    size_t endPosUser = username.size() - 4;
    std::string user = username.substr(startPosUser, endPosUser - startPosUser);
    size_t startPosPwd = password.find("\r\n\r\n") + 4;
    size_t endPosPwd = password.size() - 4;
    std::string pwd = password.substr(startPosPwd, endPosPwd - startPosPwd);

    loadUserInfo();
    std::vector<std::map<std::string, std::string> >::iterator it;
    for(it = _userInfo.begin(); it != _userInfo.end(); ++it) {
        if((*it)["username"] == user && (*it)["password"] == pwd)
            return 0;
    }
    return -1;
}

int Cookies::authUserInfo(HttpRequest &request, std::string boundary) {

    std::vector<std::string> tokens = split(request.getBody(), boundary);
    std::vector<std::string>::iterator tokenIt;
    std::string userLine = "";
    std::string pwdLine = "";

    for(tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt)
    {
        std::string line = *tokenIt;
        if(line.find("Content-Disposition: form-data; name=\"username\"") != std::string::npos)
        {
            userLine = line;
            continue;
        }
        if(line.find("Content-Disposition: form-data; name=\"password\"")!= std::string::npos)
        {
            pwdLine = line;
        }
        if (userLine != "" && pwdLine != "")
        {
            int status = checkUsernamePwd(userLine, pwdLine);
            if(status == -1)
                return -1;
            break;
        }
    }
    if (userLine == "" || pwdLine == "")
        return -1;
    return 0;
}

// sessions
std::string Cookies::generateSessionID() {
    std::stringstream ss;
    ss << std::time(0);
    for (int i = 0; i < 8; ++i) {
        ss << static_cast<char>('a' + rand() % 26);
    }
    return ss.str();
}

void Cookies::buildCookieResponse(HttpRequest& request, HttpResponse& response, int code, std::string msg) {

    if (request.getPath() == "/login" && code == 200) {
        std::string sessionId = generateSessionID();
        response.setHeaders("Set-Cookie", "session=" + sessionId + "; Path=/;");
    }
    response.setStatusLine(request.getHttpVersion(), code, msg);
    response.setHeaders("Content-Type", "text/plain");
    response.setHeaders("Content-Length", itos(msg.size()));
    response.setBody(msg);
}


Cookies::Cookies(HttpRequest &request, HttpResponse &response, std::string boundary) {


    int result = 0;
    if (request.getPath() == "/register")
    {
        std::cout << "Creating user" << std::endl;
        result = registerUserInfo(request, boundary);
        if(result == -2)
            buildCookieResponse(request, response, 409, "Username already exists");
        else if(result == -1)
            buildCookieResponse(request, response, 500, "Internal error");
        else
            buildCookieResponse(request, response, 201, "created");
    }
    else if (request.getPath() == "/login")
    {
        result = authUserInfo(request, boundary);
        if (result == -1)
            buildCookieResponse(request, response, 401, "Authentication failed");
        else if (result == 0)
            buildCookieResponse(request, response, 200, "Success");
        else
            buildCookieResponse(request, response, 500, "Internal error");
    }
}

Cookies::~Cookies() {}
