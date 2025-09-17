#include "../../../include/CGI.hpp"

/*
# Request-related
REQUEST_METHOD
QUERY_STRING
CONTENT_TYPE
CONTENT_LENGTH
PATH_INFO
PATH_TRANSLATED
SCRIPT_NAME
SCRIPT_FILENAME
REQUEST_URI
AUTH_TYPE
REMOTE_USER
REMOTE_IDENT

# Server-related
SERVER_SOFTWARE
SERVER_NAME
SERVER_ADDR
SERVER_PORT
SERVER_PROTOCOL
GATEWAY_INTERFACE

# Client-related
REMOTE_ADDR
REMOTE_PORT

# HTTP header variables (converted to environment variables)
HTTP_ACCEPT
HTTP_ACCEPT_CHARSET
HTTP_ACCEPT_ENCODING
HTTP_ACCEPT_LANGUAGE
HTTP_AUTHORIZATION
HTTP_COOKIE
HTTP_CONNECTION
HTTP_HOST
HTTP_REFERER
HTTP_USER_AGENT
HTTP_UPGRADE_INSECURE_REQUESTS
HTTP_CACHE_CONTROL
HTTP_PRAGMA
HTTP_X_FORWARDED_FOR
HTTP_X_FORWARDED_PROTO

# Misc / Non-standard / Common extensions
HTTPS
REDIRECT_STATUS
*/

void Cgi::generateErrorMsg(HttpResponse &response)
{
    std::string msg = "Oops, something went wrong :(";
    response.setStatusLine(response.getRequestAttr().httpVersion, 500, "Internal error");
    response.setHeaders("Content-Type", "text/plain");
    response.setHeaders("Content-Length", itos(msg.size()));
    response.setBody(msg);
}

void Cgi::generateResponse(HttpResponse &response)
{
    response.setStatusLine(response.getRequestAttr().httpVersion, 200, "OK");
    response.setHeaders("Content-Type", "text/plain");
    std::cout << response.getRequestAttr().path << std::endl;
    if (response.getRequestAttr().path.find("cookies") != std::string::npos) {
        response.setHeaders("Content-Length", itos(_stdout.size()));
        response.setHeaders("Set-Cookie", _stdout.substr(11));
    }
    else {
        response.setHeaders("Content-Length", itos(_stdout.size()));
        response.setBody(_stdout);
    }
}

void Cgi::createArgv(HttpResponse &response)
{
    std::string path = response.getRequestAttr().path;
    size_t questionMarkPos = path.rfind("?");
    size_t extentionDotPos = path.rfind(".");

    if(questionMarkPos == std::string::npos || questionMarkPos < extentionDotPos)
        _argv.push_back(path);
    else
        _argv.push_back(path.substr(0, questionMarkPos));
}
void Cgi::createArgvStr(std::vector<char*> &argv)
{
    for(size_t i = 0; i < _argv.size(); ++i)
        argv.push_back(const_cast<char*>(_argv[i].c_str()));
    argv.push_back(NULL);
}

void Cgi::extractOutput(int *fd)
{
    char buffer[1024];
    ssize_t bytesRead;

    close(fd[1]);
    while((bytesRead = read(fd[0], buffer, sizeof(buffer))) > 0)
        _stdout.append(buffer, bytesRead);
    close(fd[0]);
}

long Cgi::getTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec * 1000LL + tv.tv_usec / 1000);
}

void Cgi::watchdog(pid_t pid, int &status)
{
    long startTime = getTimeStamp();

    while(true)
    {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if(r == pid)
            return;
        else if (r == -1)
        {
            printError();
            return;
        }
        if(getTimeStamp() - startTime >= 3000)
        {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            return;
        }
        usleep(10 * 1000);
    }
}

Cgi::Cgi(HttpRequest &request, HttpResponse &response)
{
    createEnvp(request, response);
    createArgv(response);
    execute(request, response);
}

Cgi::~Cgi() {}
