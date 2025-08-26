#include "../include/HttpManager.hpp"

void    HttpManager::sendResponse(int clientFd)
{
    std::string response = _response.getResponse();
    ssize_t totalSent = 0;

    const char* data = response.c_str();
    ssize_t totalSize = response.size();
    while (totalSent < totalSize) {
        ssize_t sent = send(clientFd, data + totalSent, totalSize - totalSent, 0);
        if (sent < 0) {
            printError();
            break;
        }
        totalSent += sent;
    }
}

void HttpManager::writeUserInfo(HttpRequest &request, HttpResponse &response)
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

HttpManager::HttpManager(int clientFd, t_serv_attr &serverAttr)
{
    std::cout << BOLD ITALIC CYAN <<  "Request:\n" << DEFAULT;
    _request = HttpRequest(clientFd);
    if (_request.getState() == FAILURE)
        return;
    std::cout << _request << "\n";


    std::cout << BOLD ITALIC MAGENTA <<  "Response:\n" << DEFAULT;
    _response = HttpResponse(_request, serverAttr);
    sendResponse(clientFd);
    writeUserInfo(_request, _response);
    // std::cout << _response << "\n";
}

HttpManager::~HttpManager() {}
