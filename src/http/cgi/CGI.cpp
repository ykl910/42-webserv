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

void Cgi::generateErrorMsg(HttpResponse &response, int exitCode)
{
    std::string msg;

    if(exitCode == 9  || exitCode == 15)
        response.setStatusLine(response.getRequestAttr().httpVersion, 408, "Request Timeout");
    else
        response.setStatusLine(response.getRequestAttr().httpVersion, 500, "Internal Error");
    response.setHeaders("Content-Type", "text/plain");
    response.setHeaders("Content-Length", itos(0));
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
    std::cout << BOLD RED << "Entering watchdog" << DEFAULT << std::endl;
    while(true)
    {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if(r == pid)
        {
            std::cout << BOLD ORANGE << "CGI ok" << DEFAULT << std::endl;
            return;
        }
        else if (r == -1)
        {
            std::cout << BOLD ORANGE << "waitpid error" << DEFAULT << std::endl;
            printError();
            return;
        }
        if(getTimeStamp() - startTime >= 3000)
        {
            std::cout << BOLD ORANGE << "CGI timeout" << DEFAULT << std::endl;
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
