#include "../../../include/CGI.hpp"

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