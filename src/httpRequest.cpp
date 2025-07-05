#include "../include/httpRequest.hpp"

httpRequest::httpRequest(const std::string &request) {
    parse(request);
}

void    httpRequest::parse(const std::string &request)
{
    std::stringstream ss(request);
    std::string line;
    std::getline(ss, line);

    // parse request line
    std::stringstream requestLine(line);
    requestLine >> method >> path >> http_version;

    // parse headers
    while (std::getline(ss, line) && line != "\r")
    {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);
        size_t pos = line.find(": ");
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            headers[key] = value;
        }
    }

    // parse body
    std::string bodyLine;
    while (std::getline(ss, bodyLine))
    {
        body += bodyLine + "\n";
    }
}

const std::string &httpRequest::getMethod() const
{
    return method;
}

const std::string &httpRequest::getPath() const
{
    return path;
}

const std::string &httpRequest::getHttpVersion() const
{
    return http_version;
}

const std::map<std::string, std::string> &httpRequest::getHeaders() const
{
    return headers;
}

const std::string &httpRequest::getBody() const
{
    return body;
}