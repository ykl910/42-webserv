#include "../../../include/CGI.hpp"

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
    while(true)
    {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if(r == pid)
        {
            return;
        }
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
