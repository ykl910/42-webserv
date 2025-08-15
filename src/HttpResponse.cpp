#include "../include/HttpResponse.hpp"
#include "../include/GET.hpp"
#include "../include/POST.hpp"
#include "../include/DELETE.hpp"

/*
void Epoll::sendResponse(int clientFd, HttpRequest request) {

    std::string response;

    if (!_gotResponse[clientFd]) {
        HttpResponse Response(request);
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
        std::cout << BOLD ITALIC GREEN << "\nresponse:\n" << DEFAULT;
        std::cout << YELLOW << response.c_str() << std::endl;
        _pendingResponse.erase(clientFd);
        disableWriteEvent(clientFd);
        _gotFullRequest.erase(clientFd);
        _gotResponse.erase(clientFd);
        _responses.erase(clientFd);
    }
}
*/

void HttpResponse::setStatusLine(const std::string version, int code, const std::string &reason)
{
    std::ostringstream oss;
    oss << code;
    _statusLine = version + " " + oss.str() + " " + reason + "\r\n";
}

void HttpResponse::setHeaders(const std::string &key, const std::string &value)
{
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string &body)
{
    _body = body;
}

std::string HttpResponse::getStatusLine() const
{
    return _statusLine;
}

std::string HttpResponse::getResponse() const
{
    std::string fullResponse;
    fullResponse += _statusLine;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
                                                            it != _headers.end(); ++it) {
        fullResponse += it->first + ": " + it->second + "\r\n";
    }
    fullResponse += "\r\n";
    fullResponse += _body;
    return fullResponse;
}

HttpResponse::HttpResponse(HttpRequest &request)
{
    if (request.getMethod() == "GET")
        handleGet(request, *this);
    else if (request.getMethod() == "POST")
        handlePost(request, *this);
    else if (request.getMethod() == "DELETE")
        handleDelete(request, *this);
    else
        setStatusLine(request.getHttpVersion(), 405, "Method not allowed");
}
