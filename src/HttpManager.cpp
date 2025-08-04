#include "../include/HttpManager.hpp"

/*
HttpRequest    Select::readCompleteRequest(std::string *requestData, int fd, bool &success)
{
    char buf[4096];
    size_t totalBytes = 0;
    size_t contentLength = 0;
    bool headersComplete = false;
    size_t headerEnd = 0;
    while (!headersComplete)
    {
        ssize_t bytes = recv(fd, buf, sizeof(buf), 0);
        if (bytes < 0) {
            continue;
        }
        else if (bytes == 0) {
            success = false;
            return HttpRequest();
        }
        requestData->append(buf, bytes);
        totalBytes += bytes;
        headerEnd = requestData->find("\r\n\r\n");
        if (headerEnd != std::string::npos)
        {
            headersComplete = true;
            size_t clPos = requestData->find("Content-Length:");
            if (clPos != std::string::npos)
            {
                size_t valueStart = requestData->find_first_not_of(" ", clPos + 15);
                size_t valueEnd = requestData->find("\r\n", valueStart);
                std::string len = requestData->substr(valueStart, valueEnd - valueStart);
                contentLength = atoi(len.c_str());
            }
        }
    }
    while (requestData->size() < headerEnd + 4 + contentLength) {
        ssize_t bytes = recv(fd, buf, sizeof(buf), 0);
        if (bytes < 0) {
            continue;
        }
        else if (bytes == 0) {
            success = false;
            return HttpRequest();
        }
        requestData->append(buf, bytes);
    }
    HttpRequest httpReq(*requestData);
    success = true;
    return httpReq;
}

void    Select::manageRequest(int fd) {
    HttpRequest httpReq;
    std::string requestData;
    bool success = false;
    httpReq = readCompleteRequest(&requestData, fd, success);
    if (!success)
        return;
    //std::cout << BOLD ITALIC GREEN <<  "Request:\n" << DEFAULT;
    std::cout << MAGENTA << httpReq.getMethod() + " " + httpReq.getPath() << std::endl;
    //std::cout << MAGENTA << requestData << DEFAULT << std::endl;
    HttpResponse httpRes(httpReq);
    std::string response = httpRes.getResponse();
    //std::cout << BOLD ITALIC GREEN <<  "Response:\n" << DEFAULT;
    std::cout << YELLOW << httpRes.getStatusLine() << DEFAULT << std::endl;
    writeUserInfo(httpReq, httpRes);
    ssize_t totalSent = 0;
    const char* data = response.c_str();
    ssize_t totalSize = response.size();
    int retry = 10000;
    while (totalSent < totalSize && retry-- > 0) {
        ssize_t sent = send(fd, data + totalSent, totalSize - totalSent, 0);
        if (sent <= 0) {
            continue;
        }
        totalSent += sent;
    }
}
*/

HttpManager::HttpManager(int clientFd)
{
    _bytes = recv(clientFd, _buffer, sizeof(_buffer), 0);
    _request = HttpRequest(std::string(_buffer, _bytes));
    std::cout << BOLD ITALIC GREEN <<  "Received request:\n" << DEFAULT;
    // std::cout << MAGENTA << _request << DEFAULT << std::endl;

    _response = HttpResponse(_request);
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

HttpManager::~HttpManager() {}
