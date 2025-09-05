#include "../include/HttpManager.hpp"

#define CONTENT_LENGTH_SIZE 16

std::map<int, std::string>   HttpManager::_buffers;
std::map<int, HttpRequest>   HttpManager::_request;
std::map<int, bool>          HttpManager::_gotResponse;
std::map<int, bool>          HttpManager::_gotFullRequest;
std::map<int, int>           HttpManager::_pendingResponse;
std::map<int, HttpResponse>  HttpManager::_responses;

void    HttpManager::writeUserInfo(HttpRequest &request, HttpResponse &response)
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
        if (fd == -1) {
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

void    HttpManager::sendResponse(int clientFd, HttpRequest& request,
                                  t_serv_attr& servAttr) {
    std::string response;

    if (!_gotResponse[clientFd]) {
        HttpResponse Response(request, servAttr);
        writeUserInfo(request, Response);
        response = Response.getResponse();
        _responses[clientFd] = Response;
        _gotResponse[clientFd] = true;
    } else
        response = _responses[clientFd].getResponse();

    size_t totalBytesSent = _pendingResponse[clientFd];
    size_t responseLen = response.length();

    while (totalBytesSent < responseLen) {
        ssize_t bytesSent = send(clientFd, response.c_str() + totalBytesSent, responseLen - totalBytesSent, 0);
        if (bytesSent <= 0)
            break ;
        totalBytesSent += bytesSent;
    }
    if (totalBytesSent != responseLen)
        _pendingResponse[clientFd] = totalBytesSent;
    else {
        std::cout << BOLD ITALIC MAGENTA <<  "Response:\n" << DEFAULT
                  << _responses[clientFd] << "\n";
        _pendingResponse.erase(clientFd);
        _gotFullRequest.erase(clientFd);
        _gotResponse.erase(clientFd);
        _responses.erase(clientFd);
        _state = SENT;
    }
}

bool    HttpManager::receivedCompleteRequest(std::string &rawData) const {

    size_t headerEnd = rawData.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;

    size_t bodyStart = headerEnd + 4;

    size_t contentLengthPos = rawData.find("Content-Length: ");
    if (contentLengthPos == std::string::npos)
        return true;

    // size_t valueStart = contentLengthPos + "Content-Length: ";
    std::cout << std::strlen("Content-length: ") << std::endl;
    size_t valueStart = contentLengthPos + CONTENT_LENGTH_SIZE;
    size_t valueEnd = rawData.find("\r\n", valueStart);
    std::string valueStr = rawData.substr(valueStart, valueEnd - valueStart);
    int contentLength = std::atoi(valueStr.c_str());

    size_t bodyLengh = rawData.size() - bodyStart;
    return bodyLengh >= static_cast<size_t>(contentLength);
}

void    HttpManager::getRequest(int clientFd) {

    char buffer[4096];

    int bytes = recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytes >= 0)
        _buffers[clientFd].append(buffer, bytes);

    if (receivedCompleteRequest(_buffers[clientFd])) {
        HttpRequest request(_buffers[clientFd]);

        std::cout
        << BOLD ITALIC CYAN <<  "Request:\n" << DEFAULT
        << _request[clientFd].getContent();

        _request[clientFd] = request;
        _gotFullRequest[clientFd] = true;
        _buffers.erase(clientFd);
        _pendingResponse[clientFd] = 0;
        _state = RECEIVED;
    } else
        _gotFullRequest[clientFd] = false;
}

HttpManager::HttpManager(int clientFd, t_serv_attr &serverAttr, int &state)
{
    state = _state;
    getRequest(clientFd);

    if (state == RECEIVED && !_request[clientFd].getContent().empty())
        sendResponse(clientFd, _request[clientFd], serverAttr);
}

HttpManager::~HttpManager() {}
