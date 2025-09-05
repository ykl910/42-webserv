#include "../include/textFormatting.hpp"
#include "../include/HttpResponse.hpp"
#include "../include/WebServ.hpp"

void HttpResponse::handleError(HttpRequest& request, std::stringstream *buffer, int success, std::string type)
{
    std::string fullPath;
    std::string body;
    if (type == "html") {
        if (success == 404) {
            fullPath = getServerAttr().error_page.err_404;
            setStatusLine(request.getHttpVersion(), 404, "Not Found");
        } else if (success == 403) {
            fullPath = getServerAttr().error_page.err_403;
            setStatusLine(request.getHttpVersion(), 403, "Forbidden");
        } else {
            fullPath = getServerAttr().error_page.err_500;
            setStatusLine(request.getHttpVersion(), 500, "Internal error");
        }
        std::ifstream file(fullPath.c_str());
        *buffer << file.rdbuf();
        body = buffer->str();
        setHeaders("Content-Type", "text/html");
        setBody(body);
    } else if (type == "img") {
        if (success == 404)
            body = "Not Found";
        else if (success == 403)
            body = "Forbidden";
        else
            body = "Internal error";
        setStatusLine(request.getHttpVersion(), success, body);
        setHeaders("Content-Type", "text/plain");
        setHeaders("Content-Length", itos(body.length()));
        setBody(body);
    }
}

int HttpResponse::handleHtml(HttpRequest& request, std::string path, std::stringstream *buffer)
{
    std::string fullPath = POST42dotNET"html" + path;
    std::ifstream file(fullPath.c_str());
    if (access(fullPath.c_str(), F_OK) != 0)
        return 404;
    else if (access(fullPath.c_str(), R_OK) != 0)
        return 403;
    if (!file.is_open())
        return 500;
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    setStatusLine(request.getHttpVersion(), 200, "OK");
    setHeaders("Content-Type", "text/html");
    setHeaders("Content-Length", itos(body.length()));
    setBody(body);
    return 200;
}

int HttpResponse::handleCss(HttpRequest& request, std::string path, std::stringstream *buffer)
{
    std::string fullPath = POST42dotNET + path;
    std::ifstream file(fullPath.c_str());
    if (access(fullPath.c_str(), F_OK) != 0)
        return 404;
    else if (access(fullPath.c_str(), R_OK) != 0)
        return 403;
    if (!file.is_open())
        return 500;
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    setStatusLine(request.getHttpVersion(), 200, "OK");
    setHeaders("Content-Type", "text/css");
    setHeaders("Content-Length", itos(body.length()));
    setBody(body);
    return 200;
}

int HttpResponse::handleImg(HttpRequest& request, std::string path, std::string extension)
{
    std::string fullPath = POST42dotNET + path;
    std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (access(fullPath.c_str(), F_OK) != 0)
        return 404;
    else if (access(fullPath.c_str(), R_OK) != 0)
        return 403;
    if (!file.is_open())
        return 500;
    std::vector<char> data;
    char temp[4096];
    while (file.read(temp, sizeof(temp))) {
        data.insert(data.end(), temp, temp + file.gcount());
    }
    if (file.gcount() > 0)
        data.insert(data.end(), temp, temp + file.gcount());
    setStatusLine(request.getHttpVersion(), 200, "OK");
    if (extension == "png")
        setHeaders("Content-Type", "image/png");
    else if (extension == "gif")
        setHeaders("Content-Type", "image/gif");
    else if (extension == "webp")
        setHeaders("Content-Type", "image/webp");
    setHeaders("Content-Length", itos(data.size()));
    setBody(std::string(&data[0], data.size()));
    file.close();
    return 200;
}

void HttpResponse::handleThread(HttpRequest& request)
{
    std::string targetDir = POST42dotNET"threads";
    DIR* dir = opendir(targetDir.c_str());
    if (!dir) {
        setStatusLine(request.getHttpVersion(), 200, "OK");
        std::string emptyArray = "[]";
        setHeaders("Content-Type", "application/json");
        setHeaders("Content-Length", itos(emptyArray.length()));
        setBody(emptyArray);
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
    setStatusLine(request.getHttpVersion(), 200, "OK");
    setHeaders("Content-Type", "application/json");
    setHeaders("Content-Length", itos(jsonResponse.length()));
    setBody(jsonResponse);
}

static int isLogged(HttpRequest& request)
{
    std::map<std::string, std::string>::const_iterator mapit;
    std::string sessionId;
    for (mapit = request.getHeaders().begin();
         mapit != request.getHeaders().end(); ++mapit) {
        if (mapit->first == "Cookie")
            return 1;
    }
    return 0;
}

void HttpResponse::handleGET(HttpRequest& request)
{
    std::string path = request.getPath();
    if (path.find(".cgi") != std::string::npos)
        Cgi cgi(request, *this);
    else {
        if (path == "" || path == "/")
            path = "/index.html";

        std::string extension;
        size_t dot_pos = path.find_last_of(".");
        if (dot_pos != std::string::npos)
            extension = path.substr(dot_pos + 1);

        std::cout << BOLD YELLOW << extension << "\n" << DEFAULT;

        if (path == "/login.html" && isLogged(request))
            path = "/isLogged.html";

        int state = 0;
        std::stringstream buffer;
        if (request.getPath() == "/list-files") {
            handleThread(request);
        } else if (extension == "html" || extension == "htm" || extension == "") {
            state = handleHtml(request, path, &buffer);
            if (state != 200)
                handleError(request, &buffer, state, "html");
        } else if (extension == "css") {
            state = handleCss(request, path, &buffer);
            if (state != 200)
                handleError(request, &buffer, state, "img");
        } else if (extension == "png" || extension == "gif" || extension == "webp") {
            state = handleImg(request, path, extension);
            if (state != 200)
                handleError(request, &buffer, state, "img");
        } else
            handleError(request, &buffer, 500, "html");
    }
}
