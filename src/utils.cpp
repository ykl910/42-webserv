#include "../include/utils.hpp"

std::string itos(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::vector<std::string>split(std::string string, std::string &boundary)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end;

    while ((end = string.find(boundary, start)) != std::string::npos) {
        tokens.push_back(string.substr(start, end - start));
        start = end + boundary.length();
    }
    tokens.push_back(string.substr(start));
    return tokens;
}


void writeUserInfo(HttpRequest &request, HttpResponse &response)
{
    std::map<std::string, std::string>::const_iterator mapit;
    std::string sessionId;
    for (mapit = request.getHeaders().begin();
         mapit != request.getHeaders().end(); ++mapit) {
        if (mapit->first == "Cookie") {
            size_t sesStartPos = mapit->second.find("session=");
            if (sesStartPos != std::string::npos) {
                sessionId = mapit->second.substr(sesStartPos + 8, 18);
                break;
            }
        }
    }

    if (!sessionId.empty()) {
        std::string fullpath = "www/post42.net/users/sessionLog_" + sessionId;
        int fd = open(fullpath.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
        if(fd == -1) {
            printError();
            return;
        }

        for (mapit = request.getHeaders().begin();
             mapit != request.getHeaders().end(); ++mapit) {
            if (mapit->first == "Cookie") {
                std::string req = request.getMethod() + " " + request.getPath() + "\n";
                std::string res = response.getStatusLine() + "\n";
                std::string tot = "request:" + req + "response:" + res;
                write(fd, tot.c_str(), tot.length());
                break;
            }
        }
        close(fd);
    }
}