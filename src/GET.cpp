#include "../include/GET.hpp"
#include "../include/textFormatting.hpp"
#include "../include/WebServ.hpp"

void handleError(HttpRequest& request, HttpResponse& response, std::stringstream *buffer, int success, std::string type)
{
    std::string fullPath;
    std::string body;
    if (type == "html") {
        if (success == 404) {
            fullPath = "./www/website-1/html/404.html";
            response.setStatusLine(request.getHttpVersion(), 404, "Not Found");
        } else if (success == 403) {
            fullPath = "./www/website-1/html/403.html";
            response.setStatusLine(request.getHttpVersion(), 403, "Forbidden");
        } else {
            fullPath = "./www/website-1/html/500.html";
            response.setStatusLine(request.getHttpVersion(), 500, "Internal error");
        }
        std::ifstream file(fullPath.c_str());
        *buffer << file.rdbuf();
        body = buffer->str();
        response.setHeaders("Content-Type", "text/html");
    } else if (type == "img") {
        if (success == 404)
            body = "Not Found";
        else if (success == 403)
            body = "Forbidden";
        else
            body = "Internal error";
        response.setStatusLine(request.getHttpVersion(), success, body);
        response.setHeaders("Content-Type", "text/plain");
        response.setHeaders("Content-Length", itos(body.length()));
        response.setBody(body);
    }
}

int handleHtml(HttpRequest& request, HttpResponse& response, std::string path, std::stringstream *buffer)
{
    std::string fullPath = "./www/website-1/html" + path;
    std::ifstream file(fullPath.c_str());
    if (access(fullPath.c_str(), F_OK) != 0) {
        return 404;
    } else if (access(fullPath.c_str(), R_OK) != 0) {
        return 403;
    }
    if (!file.is_open())
        return 500;
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "text/html");
    response.setHeaders("Content-Length", itos(body.length()));
    response.setBody(body);
    return 200;
}

int handleCss(HttpRequest& request, HttpResponse& response, std::string path, std::stringstream *buffer)
{
    std::string fullPath = "./www/website-1" + path;
    std::ifstream file(fullPath.c_str());
    if (access(fullPath.c_str(), F_OK) != 0) {
        return 404;
    } else if (access(fullPath.c_str(), R_OK) != 0) {
        return 403;
    }
    if (!file.is_open())
        return 500;
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "text/css");
    response.setHeaders("Content-Length", itos(body.length()));
    response.setBody(body);
    return 200;
}

int handleImg(HttpRequest& request, HttpResponse& response, std::string path, std::string extension)
{
    std::string fullPath = "./www/website-1" + path;
    std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (access(fullPath.c_str(), F_OK) != 0) {
        return 404;
    } else if (access(fullPath.c_str(), R_OK) != 0) {
        return 403;
    }
    if (!file.is_open())
        return 500;
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
    file.close();
    return 200;
}

void handleThread(HttpRequest& request, HttpResponse& response)
{
    std::string targetDir = "./www/website-1/threads";
    DIR* dir = opendir(targetDir.c_str());
    if (!dir) {
        response.setStatusLine(request.getHttpVersion(), 200, "OK");
        std::string emptyArray = "[]";
        response.setHeaders("Content-Type", "application/json");
        response.setHeaders("Content-Length", itos(emptyArray.length()));
        response.setBody(emptyArray);
        return;
    }
    std::string jsonResponse = "[";
    struct dirent* entry;
    bool firstEntry = true;
    while ((entry = readdir(dir)) != NULL) {
        std::string filename = entry->d_name;
        if (filename[0] == '.')
            continue;
        else {
            if (!firstEntry)
                jsonResponse += ",";
            else
                firstEntry = false;
            jsonResponse += "\"" + filename + "\"";
        }
    }
    jsonResponse += "]";
    closedir(dir);
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "application/json");
    response.setHeaders("Content-Length", itos(jsonResponse.length()));
    response.setBody(jsonResponse);
}

int isLogged(HttpRequest& request)
{
    std::map<std::string, std::string>::const_iterator mapit;
    std::string sessionId;
    for (mapit = request.getHeaders().begin(); mapit != request.getHeaders().end(); ++mapit) {
        if (mapit->first == "Cookie")
            return 1;
    }
    return 0;
}

void handleGet(HttpRequest& request, HttpResponse& response)
{
    std::string path = request.getPath();
    if(path.find(".cgi") != std::string::npos)
        Cgi cgi(request, response);
    else {
        if (path == "" || path == "/")
            path = "/index.html";
        std::string extension;
        size_t dot_pos = path.find_last_of(".");
        if (dot_pos != std::string::npos) {
            extension = path.substr(dot_pos + 1);
        }
        if (path == "/login.html" && isLogged(request))
            path = "/isLogged.html";
        int success = 0;
        std::stringstream buffer;
        if (request.getPath() == "/list-files") {
            handleThread(request, response);
        } else if (extension == "html" || extension == "htm" || extension == "") {
            success = handleHtml(request, response, path, &buffer);
            if (success != 200)
                handleError(request, response, &buffer, success, "html");
        } else if (extension == "css") {
            success = handleCss(request, response, path, &buffer);
            if (success != 200)
                handleError(request, response, &buffer, success, "img");
        } else if (extension == "png" || extension == "gif" || extension == "webp") {
            success = handleImg(request, response, path, extension);
            if (success != 200)
                handleError(request, response, &buffer, success, "img");
        } else {
            handleError(request, response, &buffer, 500, "html");
        }
    }
}
