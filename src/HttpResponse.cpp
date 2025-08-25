#include "../include/HttpResponse.hpp"
#include "../include/Server.hpp"
#include "../include/GET.hpp"
#include "../include/POST.hpp"
#include "../include/DELETE.hpp"

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

HttpResponse::HttpResponse(HttpRequest &request, t_serv_attr &serverAttr)
{
    (void) serverAttr;
    if (request.getMethod() == "GET")
        handleGet(request, *this);
    else if (request.getMethod() == "POST")
        handlePost(request, *this);
    else if (request.getMethod() == "DELETE")
        handleDelete(request, *this);
    else
        setStatusLine(request.getHttpVersion(), 405, "Method not allowed");
}

std::ostream& operator<<(std::ostream& os, const HttpResponse& response)
{
    os << response.getResponse();
    return os;
}
