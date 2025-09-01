#include "../include/HttpRequest.hpp"
#include "../include/textFormatting.hpp"

/* REQUEST HEADERS
Accept
Accept-Charset
Accept-Encoding
Accept-Language
Authorization
Cache-Control
Connection
Content-Length
Content-Type
Cookie
Date
Expect
From
Host
If-Match
If-Modified-Since
If-None-Match
If-Range
If-Unmodified-Since
Max-Forwards
Origin
Pragma
Proxy-Authorization
Range
Referer
TE
Upgrade
User-Agent
Via
Warning
*/

const bool &HttpRequest::getState() const {
    return _state;
}

const std::string &HttpRequest::getBody() const {
    return _body;
}

const std::string &HttpRequest::getPath() const {
    return _path;
}

const std::string &HttpRequest::getMethod() const {
    return _method;
}

const std::string &HttpRequest::getContent() const {
    return _content;
}

const std::string &HttpRequest::getHttpVersion() const {
    return _http_version;
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const {
    return _headers;
}

// void    HttpRequest::getRequest(int clientFd)
// {
//     std::string line;
//     while (true) {
//         ssize_t bytes = recv(clientFd, _buffer, sizeof(_buffer), 0);
//         if (bytes == -1)
//             std::cout << BOLD RED << "Error: recv\n" << DEFAULT;

//     }
// }

void    HttpRequest::readRequest(int clientFd)
{
    size_t totalBytes = 0;
    size_t headerEnd = 0;
    size_t contentLength = 0;
    bool requestExtracted = false;

    int i = 0;
    while (!requestExtracted) {
        ssize_t bytes = recv(clientFd, _buffer, sizeof(_buffer), 0);
        if (bytes < 0)
            continue;
        else if (bytes == 0) {
            _state = FAILURE;
            return;
        }
        _content.append(_buffer, bytes);
        totalBytes += bytes;
        headerEnd = _content.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            requestExtracted = true;
            size_t contentLengthPos = _content.find("Content-Length:");
            if (contentLengthPos != std::string::npos) {
                size_t valueStart = _content.find_first_not_of(" ", contentLengthPos + 15);
                size_t valueEnd = _content.find("\r\n", valueStart);
                std::string len = _content.substr(valueStart, valueEnd - valueStart);
                contentLength = atoi(len.c_str());
            }
        }
        i++;
    }

    while (_content.size() < headerEnd + 4 + contentLength) {
        ssize_t bytes = recv(clientFd, _buffer, sizeof(_buffer), 0);
        if (bytes < 0)
            continue;
        else if (bytes == 0) {
            _state = FAILURE;
            return;
        }
        _content.append(_buffer, bytes);
    }
    if (_content.empty())
        _state = FAILURE;
    else
        _state = SUCCESS;
}

void    HttpRequest::parseRequest(void)
{
    std::stringstream ss(_content);
    std::string line;
    std::getline(ss, line);

    // parse request line
    std::stringstream requestLine(line);
    requestLine >> _method >> _path >> _http_version;

    // parse headers
    while (std::getline(ss, line) && line != "\r") {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            _headers[key] = value;
        }
    }

    // parse body
    std::string bodyLine;
    while (std::getline(ss, bodyLine)) {
        _body += bodyLine + "\n";
    }
}

HttpRequest::HttpRequest(int clientFd)
{
    readRequest(clientFd);
    parseRequest();
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& request)
{
    os << request.getContent();
    return os;
}
