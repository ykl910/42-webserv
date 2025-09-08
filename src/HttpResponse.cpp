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

std::string HttpResponse::getResponse()
{
    std::string fullResponse;
    fullResponse += _statusLine;
    for (headerMap::const_iterator it = _headers.begin();
                                   it != _headers.end(); ++it)
        fullResponse += it->first + ": " + it->second + "\r\n";
    fullResponse += "\r\n";
    _response = fullResponse;
    fullResponse += _body;
    return fullResponse;
}

std::string& HttpResponse::getResponseHeader()
{
    return _response;
}

t_serv_attr HttpResponse::getServerAttr() const
{
    return _servAttr;
}

// HttpResponse::HttpResponse(t_request_attr &request, t_serv_attr &serverAttr)
//     : _servAttr(serverAttr), _request(request)

HttpResponse::HttpResponse(HttpRequest& request, t_serv_attr &serverAttr)
    : _servAttr(serverAttr), _request(request.getRequestAttr())
{
    if(request.getBody().size() > static_cast<size_t>(serverAttr.client_max_body_size))
        setStatusLine(_request.httpVersion, 413, "Content Too Large");

    if (_request.method == "GET")
        handleGET(request);
    else if (_request.method == "POST")
        handlePOST(request);
    else if (_request.method == "DELETE")
        handleDELETE(request);
    else
        setStatusLine(_request.httpVersion, 405, "Method not allowed");
}

std::ostream& operator<<(std::ostream& os, HttpResponse& response)
{
    os << response.getResponseHeader();
    return os;
}
