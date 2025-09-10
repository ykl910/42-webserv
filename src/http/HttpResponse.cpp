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

bool    hostFound(std::string& input)
{
    (void)input;
    return true;
}

// stat
// opendir
// chdir
// readdir
// closedir

bool    HttpResponse::canAccessFile(std::string& fullPath)
{
    struct stat fileStat;

    if (stat(fullPath.c_str(), &fileStat) != 0) {
        std::cout << BOLD RED << "Can't find requested file\n" << DEFAULT;
        return false;
    }

    if (!(fileStat.st_mode & S_IRUSR)) {
        std::cout << BOLD RED << "Don't have read permissions\n" << DEFAULT;
        return false;
    }

    return true;
}

void    HttpResponse::solvePath()
{
    std::string fullPath;

    std::cout << "Request PATH: " << _request.path << "\n";
    std::cout << "Server location: " << _server.location[0].path << "\n";
    std::cout << BOLD CYAN << _request.path << std::endl << DEFAULT;
    size_t dot_pos = _request.path.find_last_of(".");
    if (dot_pos != std::string::npos)
        _extension = _request.path.substr(dot_pos + 1);
    //std::cout << BOLD YELLOW << _extension << "\n" << DEFAULT;

    if (_request.path == "" || _request.path == "/")
        fullPath = _server.location[0].root + "/html/" + _server.location[0].index;
    else {
        if (_extension == "html")
            fullPath = _server.location[0].root + "/html" + _request.path;
        else if (_request.path[0] == '/')
            fullPath = _server.location[0].root + _request.path;
    }
    _request.path = fullPath;
    //std::cout << BOLD BLUE << _request.path << std::endl << DEFAULT;
}

bool    HttpResponse::isImage()
{
    return _extension == "jpg" || _extension == "jpeg" || _extension == "png"
        || _extension == "gif" || _extension == "bmp" || _extension == "tif"
        || _extension == "tiff" || _extension == "ico" || _extension == "webp"
        || _extension == "avif" || _extension == "svg" || _extension == "apng"
        || _extension == "heic" || _extension == "heif";
}

bool    HttpResponse::isAudio()
{
    return _extension == "mp3" || _extension == "wav" || _extension == "ogg"
        || _extension == "oga" || _extension == "m4a" || _extension == "aac"
        || _extension == "flac" || _extension == "opus";
}

void    HttpResponse::solveMimeType()
{
    return;
}

const std::string HttpResponse::getFileExtention(const std::string &path) const
{
    size_t questionMarkPos = path.rfind("?");
    size_t extentionDotPos = path.rfind(".");

    if(questionMarkPos == std::string::npos || questionMarkPos < extentionDotPos)
        return path.substr(extentionDotPos, path.size());
    else
        return path.substr(extentionDotPos, questionMarkPos - extentionDotPos);
}

bool HttpResponse::isCgi(std::string &requestPath)
{
    std::string fileExtention = getFileExtention(requestPath);

    //!waiting for implementation of cgi structs
   //for(int i = 0; _server.cgi[i]; ++i)
   //{
   //    if(_server.cgi[i].name == fileExtention)
   //        return true;
   //}
   return false;
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
        return bodySize > serverAttr.client_max_body_size;
    }
}

HttpResponse::HttpResponse(HttpRequest& request, t_serv_attr &serverAttr)
    : _server(serverAttr), _request(request.getRequestAttr())
{
    if(!isValidBodySize(request, serverAttr))
        setStatusLine(_request.httpVersion, 413, "Content Too Large");
    solvePath();
    solveMimeType();

    std::string ext = getFileExtention(_request.path);
    std::cout << RED BOLD << "File extention: " << ext << DEFAULT << std::endl;

    //!waiting for implementation of cgi structs
    //if(isCgi(_request.path))
    //    Cgi cgi(request, *this);

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
