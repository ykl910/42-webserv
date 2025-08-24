#include "../include/HttpManager.hpp"
#include "../include/Server.hpp"
#include "../include/utils.hpp"

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

HttpManager::HttpManager(int clientFd, t_serv_attr &serverAttr)
{
    std::cout << BOLD ITALIC GREEN <<  "Request:\n" << DEFAULT;
    // std::cout << MAGENTA << _request << DEFAULT << std::endl;
    _request = HttpRequest(clientFd);
    if (_request.getState() == FAILURE)
        return;


    _response = HttpResponse(_request, serverAttr);
    sendResponse(clientFd);
    writeUserInfo(_request, _response);
}

HttpManager::~HttpManager() {}
