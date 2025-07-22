#include "../include/CGI.hpp"
#include "../include/utils.hpp"

void Cgi::generateErrorMsg(HttpRequest &request, HttpResponse &response) {
    std::string msg = "Oops, something went wrong :(";
    response.setStatusLine(request.getHttpVersion(), 500, "Internal error");
    response.setHeaders("Content-Type", "text/plain");
    response.setHeaders("Content-Length", itos(msg.size()));
    response.setBody(msg);
}

void Cgi::generateResponse(HttpRequest &request, HttpResponse &response) {
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "text/plain");
    response.setHeaders("Content-Length", itos(this->_stdout.size()));
    response.setBody(this->_stdout);
}

std::string Cgi::extractQuery(HttpRequest &request) {

    std::string header = request.getPath();
    size_t varStart = header.find('?') + 1;
    size_t varEnd = header.size();
    std::string var = header.substr(varStart, varEnd - varStart);
    std::cout << "extract var: " << var << std::endl;

    return var;
}

std::vector<char*> Cgi::createEnv(HttpRequest &request) {

    std::vector<std::string> temp;
    std::vector<char*> envp;

    temp.push_back("REQUEST_METHOD=" + request.getMethod());
    temp.push_back("QUERY_STRING=" + extractQuery(request));
    temp.push_back("SCRIPT_NAME=" + request.getPath());

    //TODO : CONTENT_LENGTH= + CONTENT_TYPE= pour POST

    for(size_t i = 0; i < temp.size(); ++i)
        envp.push_back(const_cast<char*>(temp[i].c_str()));
    envp.push_back(NULL);

    return envp;
}

void Cgi::extractOutput(int *fd) {

    char buffer[1024];
    ssize_t bytesRead;

    close(fd[1]);
    while((bytesRead = read(fd[0], buffer, sizeof(buffer))) > 0)
        this->_stdout.append(buffer, bytesRead);
    close(fd[0]);
}

int Cgi::execFromGet(HttpRequest &request) {

    int fds[2];
    if(pipe(fds) == -1)
    {
        printError();
        return EXIT_FAILURE;
    }
    pid_t pid = fork();
    if(pid == -1)
    {
        printError();
        return EXIT_FAILURE;
    }
    else if (pid == 0)
    {
        std::vector<std::string> temp;
        std::vector<char*> envp;

        temp.push_back("REQUEST_METHOD=" + request.getMethod());
        temp.push_back("QUERY_STRING=" + extractQuery(request));
        temp.push_back("SCRIPT_NAME=" + request.getPath());

        //TODO : CONTENT_LENGTH= + CONTENT_TYPE= pour POST

        for(size_t i = 0; i < temp.size(); ++i)
            envp.push_back(const_cast<char*>(temp[i].c_str()));
        envp.push_back(NULL);

        char* argv[] = {const_cast<char*>("./cgi/bin/roulette.cgi"), NULL};

        if(dup2(fds[1], STDOUT_FILENO) == -1)
        {
            printError();
            exit(EXIT_FAILURE);
        }
        close(fds[0]);
        close(fds[1]);
        execve("./cgi/bin/roulette.cgi", argv, envp.data());
        printError();
        exit(EXIT_FAILURE);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        extractOutput(fds);
        return WEXITSTATUS(status);
    }
}

int Cgi::execFromPost(HttpRequest &request) {
    (void)request;
    //TODO
    return EXIT_SUCCESS;
}

void Cgi::execute(HttpRequest &request, HttpResponse &response) {

    if(request.getMethod() == "GET")
    {
        if(execFromGet(request) == EXIT_FAILURE)
            generateErrorMsg(request, response);
        else
            generateResponse(request, response);
    }
    if(request.getMethod() == "POST")
    {
        if(execFromPost(request) == EXIT_FAILURE)
            generateErrorMsg(request, response);
        else
            generateResponse(request, response);
    }
}

Cgi::Cgi(HttpRequest &request, HttpResponse &response) {
    this->execute(request, response);
}

Cgi::~Cgi() {

}
