#include "../include/ResponseHandler.hpp"

std::string itos(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void handleGet(HttpRequest& request, HttpResponse& response) {

    std::string root = "./website/html";
    std::string path = request.getPath();
    if (path == "" || path == "/")
        path = "index.html";
    std::string fullPath = root + path;
    std::ifstream file(fullPath.c_str());
    int fd = open(fullPath.c_str(),O_RDONLY);
    if (!fd)
    {
        std::string reason = "page not found";
        response.setStatusLine(request.getHttpVersion(), 404, reason);
        return;
    }
    else
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string body = buffer.str();
        response.setStatusLine(request.getHttpVersion(), 200, "ok");
        response.setHeaders("Content-Type", "text/html");
        response.setHeaders("Content-Length", itos(body.length()));
        response.setBody(buffer.str());
        close(fd);
    }
}


void buildGetResponse(HttpRequest& request, HttpResponse& response)
{
    
}

void handlePost(HttpRequest& request, HttpResponse& response) {
    (void)request;
    (void)response;
}
void handleDelete(HttpRequest& request, HttpResponse& response) {
    (void)request;
    (void)response;
}
