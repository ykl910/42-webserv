#include "../include/HttpRequest.hpp"

void    HttpRequest::parse(const std::string &request)
{
    std::stringstream ss(request);
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

HttpRequest::HttpRequest(const std::string &request)
{
    parse(request);
}
