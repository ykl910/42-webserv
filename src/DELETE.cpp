#include "../include/textFormatting.hpp"
#include "../include/HttpResponse.hpp"
#include "../include/WebServ.hpp"

void HttpResponse::handleDelete(HttpRequest& request)
{
    std::string fullPath = POST42dotNET + request.getPath();
    if (fullPath == POST42dotNET"threads/")
        fullPath = POST42dotNET"threads/nothingSelected";
    std::string body;
    if (access(fullPath.c_str(), F_OK) != 0) {
        body = "Not Found";
        setStatusLine(request.getHttpVersion(), 404, body);
    } else if (access(fullPath.c_str(), W_OK) != 0) {
        body = "Forbidden";
        setStatusLine(request.getHttpVersion(), 403, body);
    } else if (unlink(fullPath.c_str()) == 0) {
        body = "File deleted successfully";
        setStatusLine(request.getHttpVersion(), 200, body);
    } else {
        body = "Internal Server Error";
        setStatusLine(request.getHttpVersion(), 500, body);
    }
    setHeaders("Content-Type", "text/plain");
    setHeaders("Content-Length", itos(body.length()));
    setBody(body);
}
