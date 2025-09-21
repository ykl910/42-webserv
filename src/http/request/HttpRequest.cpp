#include "../../../include/HttpRequest.hpp"
#include "../../../include/textFormatting.hpp"

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

const std::map<std::string, std::string> &HttpRequest::getHeaders() {
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

    // extract request line
    std::stringstream requestLine(line);
    requestLine >> _attributes.method
                >> _attributes.path
                >> _attributes.httpVersion;
    _content = std::string(_attributes.method + " ");
    _content += _attributes.path + " ";
    _content += _attributes.httpVersion + "\n";

    // extract headers
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

    // extract body
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
