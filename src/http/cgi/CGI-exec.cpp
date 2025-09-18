#include "../../../include/CGI.hpp"

std::string Cgi::getBinDir(std::string binPath)
{
    size_t substrEndPos = binPath.rfind("/") + 1;
    return binPath.substr(0, substrEndPos);
}

std::string Cgi::getBinName(std::string binPath)
{
    size_t substrStartPos = binPath.rfind("/");

    return "." + binPath.substr(substrStartPos, binPath.size() - substrStartPos);
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


        if(chdir(getBinDir(_argv.front()).c_str()) == -1)
            exit(126);

        std::cerr << BOLD ORANGE << "script path: " << argvStr.front() << DEFAULT << std::endl;

        execve(getBinName(argvStr.front()).c_str(), argvStr.data(), envpStr.data());
        printError();
        exit(EXIT_FAILURE);

    } else {
        int status;
        watchdog(pid, status);
        extractOutput(fds);
        if(WIFSIGNALED(status))
            return WTERMSIG(status);
        else if(WIFEXITED(status))
            return WEXITSTATUS(status);
        else
            return EXIT_FAILURE;
    }
}

int Cgi::execFromPost(HttpRequest &request)
{
    int inputPipe[2];
    int outputPipe[2];

    if (pipe(inputPipe) == -1 || pipe(outputPipe) == -1) {
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
        if (dup2(inputPipe[0], STDIN_FILENO) == -1) {
            printError();
            exit(EXIT_FAILURE);
        }

        if (dup2(outputPipe[1], STDOUT_FILENO) == -1) {
            printError();
            exit(EXIT_FAILURE);
        }
        close(inputPipe[0]);
        close(inputPipe[1]);
        close(outputPipe[0]);
        close(outputPipe[1]);

        if(chdir(getBinDir(_argv.front()).c_str()) == -1)
           exit(126);

        std::cerr << BOLD ORANGE << "script path: " << argvStr.front() << DEFAULT << std::endl;
        execve(getBinName(argvStr.front()).c_str(), argvStr.data(), envpStr.data());
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
        if(WIFSIGNALED(status))
            return WTERMSIG(status);
        else if(WIFEXITED(status))
            return WEXITSTATUS(status);
        else
            return EXIT_FAILURE;
    }
}

void Cgi::execute(HttpRequest &request, HttpResponse &response)
{
    int exitCode;

    if (response.getRequestAttr().method == "GET")
        exitCode = execFromGet();
    else
        exitCode = execFromPost(request);

    if (exitCode != EXIT_SUCCESS)
        generateErrorMsg(response, exitCode);
    else
        generateResponse(response);
}
