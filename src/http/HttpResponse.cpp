#include "../../include/HttpResponse.hpp"
#include "../../include/Server.hpp"

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
    return _server;
}

t_request_attr HttpResponse::getRequestAttr() const
{
    return _request;
}

const std::string HttpResponse::getFileExtention(const std::string &path) const
{
    size_t questionMarkPos = path.rfind("?");
    size_t extentionDotPos = path.rfind(".");

    if(questionMarkPos == std::string::npos || questionMarkPos < extentionDotPos)
        return path.substr(extentionDotPos, path.size() - extentionDotPos);
    else
        return path.substr(extentionDotPos, questionMarkPos - extentionDotPos);
}

bool HttpResponse::isCgi(std::string &requestPath)
{
    std::string fileExtention = getFileExtention(requestPath);

   return _server.cgi.find(fileExtention) != _server.cgi.end();
}

bool HttpResponse::isValidBodySize(HttpRequest &request, t_serv_attr &serverAttr) const
{
    std::map<std::string, std::string>header = request.getHeaders();
    std::map<std::string, std::string>::iterator it = header.find("Content-Length");
    if(it->second.empty())
        return true;
    else
    {
        int bodySize =  std::atoi(header["Content-Length"].c_str());
        return bodySize < serverAttr.client_max_body_size;
    }
}

#define GET 0
#define POST 1
#define DELETE 2

HttpResponse::HttpResponse(HttpRequest& request, t_serv_attr &serverAttr)
    : _server(serverAttr), _request(request.getRequestAttr())
{
    if(!isValidBodySize(request, serverAttr))
        setStatusLine(_request.httpVersion, 413, "Content Too Large");

    buildResponse();
    if (isCgi(_request.path))
        Cgi cgi(request, *this);
    else if (_request.method == "GET" && (_allowedMethod & (1 << GET)))
        handleGET();
    else if (_request.method == "POST" && (_allowedMethod & (1 << POST)))
        handlePOST(request);
    else if (_request.method == "DELETE" && (_allowedMethod & (1 << DELETE)))
        handleDELETE();
    else
    {
        std::stringstream buffer;
        std::string body;
        std::ifstream file(_server.error_page.err_405.c_str());
        buffer << file.rdbuf();
        body = buffer.str();
        setBody(body);
        setHeaders("Content-Type", "text/html");
        setHeaders("Content-Length", itos(body.length()));
        setStatusLine(_request.httpVersion, 405, "Method not allowed");
    }

}

std::ostream& operator<<(std::ostream& os, HttpResponse& response)
{
    os << response.getResponseHeader();
    return os;
}
