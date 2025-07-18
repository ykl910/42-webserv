#include "../include/DELETE.hpp"
#include "../include/textFormatting.hpp"
#include "../include/WebServ.hpp"

void handleDelete(HttpRequest& request, HttpResponse& response) {
    std::string fullPath = "./website" + request.getPath();
    std::string body;
    response.setHeaders("Content-Type", "text/plain");
    if (access(fullPath.c_str(), F_OK) != 0) {
        body = "Not Found";
        response.setStatusLine(request.getHttpVersion(), 404, body);
    }
    else if (access(fullPath.c_str(), W_OK) != 0) {
        body = "Forbidden";
        response.setStatusLine(request.getHttpVersion(), 403, body);
    }
    else if (unlink(fullPath.c_str()) == 0) {
        body = "File deleted successfully.";
        response.setStatusLine(request.getHttpVersion(), 200, body);
    }
    else {
        body = "Internal Server Error";
        response.setStatusLine(request.getHttpVersion(), 500, body);
    }
    response.setHeaders("Content-Length", itos(body.length()));
    response.setBody(body);
}
