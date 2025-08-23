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
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                printError();
                break;
            }
        }
        totalSent += sent;
    }
}

HttpManager::HttpManager(int clientFd)
{
    std::cout << BOLD ITALIC GREEN <<  "Request:\n" << DEFAULT;
    // std::cout << MAGENTA << _request << DEFAULT << std::endl;
    _request = HttpRequest(clientFd);
    if (_request.getState() == FAILURE)
        return;


    _response = HttpResponse(_request);
    sendResponse(clientFd);
}

HttpManager::~HttpManager() {}
