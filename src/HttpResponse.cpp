#include "../include/HttpResponse.hpp"
#include "../include/Server.hpp"

/* RESPONSE HEADERS
Accept-Ranges
Age
Allow
Cache-Control
Connection
Content-Disposition
Content-Encoding
Content-Language
Content-Length
Content-Location
Content-Range
Content-Type
Date
ETag
Expires
Last-Modified
Link
Location
Pragma
Proxy-Authenticate
Retry-After
Server
Set-Cookie
Trailer
Transfer-Encoding
Vary
Via
Warning
WWW-Authenticate
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

t_serv_attr HttpResponse::getServerAttr() const
{
    return _servAttr;
}

HttpResponse::HttpResponse(HttpRequest &request, t_serv_attr &serverAttr)
{
    _servAttr = serverAttr;
    std::string method = request.getMethod();

    if (method == "GET") {
        std::cout << "GET\n";
        handleGet(request);
    }
    else if (method == "POST") {
        std::cout << "Post\n";
        handlePost(request);
    }
    else if (method == "DELETE") {
        std::cout << "Delete\n";
        handleDelete(request);
    }
    else
        setStatusLine(request.getHttpVersion(), 405, "Method not allowed");
}

std::ostream& operator<<(std::ostream& os, const HttpResponse& response)
{
    os << response.getResponse();
    return os;
}
