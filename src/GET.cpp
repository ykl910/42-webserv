#include "../include/textFormatting.hpp"
#include "../include/HttpResponse.hpp"
#include "../include/WebServ.hpp"

void HttpResponse::handleError(std::stringstream *buffer, int success, std::string type)
{
    std::string fullPath;
    std::string body;
    if (type == "html") {
        if (success == 404) {
            fullPath = getServerAttr().error_page.err_404;
            setStatusLine(_request.httpVersion, 404, "Not Found");
        } else if (success == 403) {
            fullPath = getServerAttr().error_page.err_403;
            setStatusLine(_request.httpVersion, 403, "Forbidden");
        } else {
            fullPath = getServerAttr().error_page.err_500;
            setStatusLine(_request.httpVersion, 500, "Internal error");
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
        setStatusLine(_request.httpVersion, success, body);
        setHeaders("Content-Type", "text/plain");
        setHeaders("Content-Length", itos(body.length()));
        setBody(body);
    }
}

int HttpResponse::handleHtml(HttpRequest& request, std::stringstream *buffer)
{
    std::cout << BOLD BLUE << _request.path << DEFAULT << std::endl;
    std::ifstream file(_request.path.c_str());
    if (access(_request.path.c_str(), F_OK) != 0)
        return 404;
    else if (access(_request.path.c_str(), R_OK) != 0)
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

int HttpResponse::handleCss(HttpRequest& request, std::stringstream *buffer)
{
    std::ifstream file(_request.path.c_str());

    if (access(_request.path.c_str(), F_OK) != 0)
        return 404;
    else if (access(_request.path.c_str(), R_OK) != 0)
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

int HttpResponse::handleImg(HttpRequest& request)
{
    std::ifstream file(_request.path.c_str(), std::ios::in | std::ios::binary);

    if (access(_request.path.c_str(), F_OK) != 0)
        return 404;
    else if (access(_request.path.c_str(), R_OK) != 0)
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
    if (_extension == "png")
        setHeaders("Content-Type", "image/png");
    else if (_extension == "gif")
        setHeaders("Content-Type", "image/gif");
    else if (_extension == "webp")
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
        setStatusLine(_request.httpVersion, 200, "OK");
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
    if (_request.path.find(".cgi") != std::string::npos)
        Cgi cgi(request, *this);

    else {
        if (_request.path == "/login.html" && isLogged(request))
            _request.path = "/isLogged.html";

        int state = 0;
        std::stringstream buffer;
        // if (_request.path == "/list-files") {
        //     handleThread(request);
        routeRequest();
        if (_extension == "html" || _extension == "htm" || _extension == "") {
            state = handleHtml(request, &buffer);
            if (state != 200)
                handleError(&buffer, state, "html");

        } else if (_extension == "css") {
            state = handleCss(request, &buffer);
            if (state != 200)
                handleError(&buffer, state, "img");

        } else if (_extension == "png" || _extension == "gif" || _extension == "webp") {
            state = handleImg(request);
            if (state != 200)
                handleError(&buffer, state, "img");

        } else
            handleError(&buffer, 500, "html");
    }
}
