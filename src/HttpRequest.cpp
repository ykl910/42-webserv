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

const t_request_attr& HttpRequest::getRequestAttr() const {
    return _attributes;
}

void    HttpRequest::extractRequest(const std::string &request)
{
    std::stringstream ss(request);
    std::string line;
    std::getline(ss, line);

    // parse request line
    std::stringstream requestLine(line);
    requestLine >> _method >> _path >> _http_version;
    _content = std::string(_method + " ");
    _content += _path + " ";
    _content += _http_version + "\n";
    _attributes.method = _method;
    _attributes.path = _path;
    _attributes.httpVersion = _http_version;

    // parse headers
    while (std::getline(ss, line) && line != "\r") {

        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);

        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            _headers[key] = value;
            _content += key + ": ";
            _content += value + "\n";
        }
    }

    // parse body
    std::string bodyLine;
    while (std::getline(ss, bodyLine)) {
        _body += bodyLine + "\n";
    }
    _content += _body + "\n";
}

HttpRequest::HttpRequest(const std::string& request)
{
    extractRequest(request);
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& request)
{
    os << request.getContent();
    return os;
}
