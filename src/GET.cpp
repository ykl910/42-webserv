#include "../include/GET.hpp"

std::string itos(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

bool handleHtmlError(HttpRequest& request, HttpResponse& response, std::stringstream *buffer) {
    std::string fullPath = "./website/html/404.html";
    std::ifstream file(fullPath.c_str());
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    response.setStatusLine(request.getHttpVersion(), 404, "Not Found");
    response.setHeaders("Content-Type", "text/html");
    response.setHeaders("Content-Length", itos(body.length()));
    response.setBody(body);
    return true;
}

bool handleHtml(HttpRequest& request, HttpResponse& response, std::string path, std::stringstream *buffer) {
    std::string fullPath = "./website/html" + path;
    std::ifstream file(fullPath.c_str());
    if (!file.is_open())
        return false;
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "text/html");
    response.setHeaders("Content-Length", itos(body.length()));
    response.setBody(body);
    return true;
}

bool handleCss(HttpRequest& request, HttpResponse& response, std::string path, std::stringstream *buffer) {
    std::string fullPath = "./website/html" + path;
    std::ifstream file(fullPath.c_str());
    if (!file.is_open())
    {
        handleCss(request, response, "/style.css", buffer);
        return true;
    }
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "text/css");
    response.setHeaders("Content-Length", itos(body.length()));
    response.setBody(body);
    return true;
}

bool handleImg(HttpRequest& request, HttpResponse& response, std::string path, std::string extension) {
    std::string fullPath = "./website" + path;
    std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        response.setStatusLine(request.getHttpVersion(), 404, "Not Found");
        std::cerr << "Image not found for path: " << fullPath << std::endl;
        return false;
    }
    std::vector<char> data;
    char temp[4096];
    while (file.read(temp, sizeof(temp))) {
        data.insert(data.end(), temp, temp + file.gcount());
    }
    if (file.gcount() > 0) {
        data.insert(data.end(), temp, temp + file.gcount());
    }
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    if (extension == "png")
        response.setHeaders("Content-Type", "image/png");
    else if (extension == "gif")
        response.setHeaders("Content-Type", "image/gif");
    else if (extension == "webp")
        response.setHeaders("Content-Type", "image/webp");
    response.setHeaders("Content-Length", itos(data.size()));
    response.setBody(std::string(&data[0], data.size()));
    return true;
}

void handleGet(HttpRequest& request, HttpResponse& response) {
    std::string root = "./website";
    std::string path = request.getPath();
    if (path == "" || path == "/")
        path = "/index.html";
    std::string extension;
    size_t dot_pos = path.find_last_of(".");
    if (dot_pos != std::string::npos) {
        extension = path.substr(dot_pos + 1);
    }
    bool success = false;
    std::stringstream buffer;
    if (extension == "html" || extension == "htm" || extension == "") {
        success = handleHtml(request, response, path, &buffer);
        if (!success)
            handleHtmlError(request, response, &buffer);
    } else if (extension == "css") {
        success = handleCss(request, response, path, &buffer);
    } else if (extension == "png" || extension == "gif" || extension == "webp") {
        success = handleImg(request, response, path, extension);
    }
}
