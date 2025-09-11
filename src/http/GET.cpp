#include "../../include/textFormatting.hpp"
#include "../../include/HttpResponse.hpp"
#include "../../include/WebServ.hpp"

void HttpResponse::handleError(std::stringstream *buffer, int success, std::string type)
{
    std::string fullPath;
    std::string body;
    if (type == "html") {
        if (success == 404) {
            fullPath = _server.error_page.err_404;
            std::cout << "full error page: " << fullPath << std::endl;
            setStatusLine(_request.httpVersion, 404, "Not Found");
        } else if (success == 403) {
            fullPath = _server.error_page.err_403;
            setStatusLine(_request.httpVersion, 403, "Forbidden");
        } else {
            fullPath = _server.error_page.err_500;
            setStatusLine(_request.httpVersion, 500, "Internal error");
        }
        std::ifstream file(fullPath.c_str());
        *buffer << file.rdbuf();
        body = buffer->str();
        setHeaders("Content-Type", "text/html");
    } else if (type == "img") {
        if (success == 404)
            body = "Not Found";
        else if (success == 403)
            body = "Forbidden";
        else
            body = "Internal error";
        setStatusLine(_request.httpVersion, success, body);
        setHeaders("Content-Type", "text/plain");
    }
    setHeaders("Content-Length", itos(body.length()));
    setBody(body);    
}

int HttpResponse::handleRedirect()
{
    std::ifstream file(_request.path.c_str());
    if (_request.path == _server.redirection.redir_301[0])
    {
        setStatusLine(_request.httpVersion, 301, "Moved Permanently");
        setHeaders("Location", _server.redirection.redir_301[1]);
        setHeaders("Content-Length", "0");
        setBody("");
        return 0;
    } else if (_request.path == _server.redirection.redir_302[0])
    {
        setStatusLine(_request.httpVersion, 302, "Moved Temporarily");
        setHeaders("Location", _server.redirection.redir_302[1]);
        setHeaders("Content-Length", "0");
        setBody("");
        return 0;
    }
    return 1;
}

int HttpResponse::handleHtml(std::stringstream *buffer)
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
    setStatusLine(_request.httpVersion, 200, "OK");
    setHeaders("Content-Type", "text/html");
    setHeaders("Content-Length", itos(body.length()));
    setBody(body);
    return 200;
}

int HttpResponse::handleCss(std::stringstream *buffer)
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
    setStatusLine(_request.httpVersion, 200, "OK");
    setHeaders("Content-Type", "text/css");
    setHeaders("Content-Length", itos(body.length()));
    setBody(body);

    return 200;
}

int HttpResponse::handleImage()
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

    setStatusLine(_request.httpVersion, 200, "OK");
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

void HttpResponse::handleGET()
{
    int state = 0;
    std::stringstream buffer;
    if (!handleRedirect())
        return;
    if (_extension == "html" || _extension == "htm" || _extension == "") {
        state = handleHtml(&buffer);
        if (state != 200)
            handleError(&buffer, state, "html");
    } else if (_extension == "css") {
        state = handleCss(&buffer);
        if (state != 200)
            handleError(&buffer, state, "css");
    } else if (isImage()) {
        state = handleImage();
        if (state != 200)
            handleError(&buffer, state, "img");
    } else
        handleError(&buffer, 500, "html");
}
