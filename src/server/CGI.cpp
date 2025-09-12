#include "../../include/CGI.hpp"

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
    response.setHeaders("Content-Length", itos(_stdout.size()));
    response.setBody(_stdout);
}

std::string Cgi::extractQuery(HttpResponse &response)
{
    std::string header = response.getRequestAttr().path;
    size_t start = header.rfind('?') + 1;
    std::string var = header.substr(start, header.size() - start);

    return var;
}

void Cgi::createEnvp(HttpRequest &request, HttpResponse &response)
{
    std::string method = response.getRequestAttr().method;

    _envp.push_back("REQUEST_METHOD=" + method);
    _envp.push_back("SCRIPT_NAME=" + response.getRequestAttr().path);
    if(method == "GET")
        _envp.push_back("QUERY_STRING=" + extractQuery(response));
    else
    {
        std::map<std::string, std::string> header = request.getHeaders();

        _envp.push_back("CONTENT_LENGTH=" + header["Content-Length"]);
        _envp.push_back("CONTENT_TYPE=" + header["Content-Type"]);
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

void Cgi::createEnvpStr(std::vector<char*> &envp)
{
    for(size_t i = 0; i < _envp.size(); ++i)
        envp.push_back(const_cast<char*>(_envp[i].c_str()));
    envp.push_back(NULL);
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
int Cgi::execFromGet()
{
    int fds[2];
    if (pipe(fds) == -1) {
        printError();
        return EXIT_FAILURE;
    }
    pid_t pid = fork();
    if (pid == -1) {
        printError();
        return EXIT_FAILURE;
    } else if (pid == 0) {

        std::vector<char*> envpStr;
        std::vector<char*> argvStr;

        createEnvpStr(envpStr);
        createArgvStr(argvStr);

        if(dup2(fds[1], STDOUT_FILENO) == -1) {
            printError();
            exit(EXIT_FAILURE);
        }

        close(fds[0]);
        close(fds[1]);
        execve(argvStr.front(), argvStr.data(), envpStr.data());
        printError();
        exit(EXIT_FAILURE);
    } else {
        int status;
        watchdog(pid, status);
        extractOutput(fds);
        return WEXITSTATUS(status);
    }
}

int Cgi::execFromPost(HttpRequest &request)
{
    int inputPipe[2];
    int outputPipe[2];

    if(pipe(inputPipe) == -1 || pipe(outputPipe) == -1) {
        printError();
        return EXIT_FAILURE;
    }
    pid_t pid = fork();
    if(pid == -1) {
        printError();
        return EXIT_FAILURE;
    } else if (pid == 0) {
        std::vector<char*> envpStr;
        std::vector<char*> argvStr;

        createEnvpStr(envpStr);
        createArgvStr(argvStr);
        if(dup2(inputPipe[0], STDIN_FILENO) == -1) {
            printError();
            exit(EXIT_FAILURE);
        }
        if(dup2(outputPipe[1], STDOUT_FILENO) == -1) {
            printError();
            exit(EXIT_FAILURE);
        }
        close(inputPipe[0]);
        close(inputPipe[1]);
        close(outputPipe[0]);
        close(outputPipe[1]);

        execve(argvStr.front(), argvStr.data(), envpStr.data());
        printError();
        exit(EXIT_FAILURE);
    } else {
        std::string body = request.getBody();

        close(inputPipe[0]);
        write(inputPipe[1], body.c_str(), body.size());
        close(inputPipe[1]);

        int status;
        watchdog(pid, status);
        extractOutput(outputPipe);
        return WEXITSTATUS(status);
    }
}

void Cgi::execute(HttpRequest &request, HttpResponse &response)
{
    if (response.getRequestAttr().method == "GET") {
        if(execFromGet() == EXIT_FAILURE)
            generateErrorMsg(response);
        else
            generateResponse(response);
    }
    if (response.getRequestAttr().method == "POST") {
        if(execFromPost(request) == EXIT_FAILURE)
            generateErrorMsg(response);
        else
            generateResponse(response);
    }
}

Cgi::Cgi(HttpRequest &request, HttpResponse &response)
{
    createEnvp(request, response);
    createArgv(response);
    execute(request, response);
}

Cgi::~Cgi() {}
