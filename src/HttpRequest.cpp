#include "../include/HttpRequest.hpp"

const bool &HttpRequest::getState() const {
    return _state;
}

const std::string &HttpRequest::getMethod() const {
    return _method;
}

const std::string &HttpRequest::getPath() const {
    return _path;
}

const std::string &HttpRequest::getHttpVersion() const {
    return _http_version;
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const {
    return _headers;
}

const std::string &HttpRequest::getBody() const {
    return _body;
}

void    HttpRequest::readRequest(int clientFd)
{
    size_t totalBytes = 0;
    size_t headerEnd = 0;
    size_t contentLength = 0;
    bool headersComplete = false;

    while (!headersComplete) {
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
            headersComplete = true;
            size_t clPos = _content.find("Content-Length:");
            if (clPos != std::string::npos) {
                size_t valueStart = _content.find_first_not_of(" ", clPos + 15);
                size_t valueEnd = _content.find("\r\n", valueStart);
                std::string len = _content.substr(valueStart, valueEnd - valueStart);
                contentLength = atoi(len.c_str());
            }
        }
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
