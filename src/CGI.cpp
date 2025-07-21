#include "../include/CGI.hpp"

void Cgi::generateErrorMsg(HttpRequest &request, HttpResponse &response) {
    response.setHeaders("Content-Type", "text/html\n");
    response.setStatusLine(request.getHttpVersion(), 500, "Internal error");
    response.setBody("Oops, something went wrong :(");
}

int Cgi::execFromGet(HttpRequest &request, HttpResponse &response) {

    int fd[2];
    pid_t pid = fork();
    std::string header = request.getPath();
    size_t varStart = header.find('?') + 1;
    size_t varEnd = header.size();
    std::string var = header.substr(varStart, varEnd - varStart);
    char* envp[] = { const_cast<char*>(var.c_str()), NULL};
    char* argv[] = { const_cast<char*>("./cgi-bin/multiplication.cgi"), NULL};

    if(pid == -1 || pipe(fd) == -1)
        return EXIT_FAILURE;
    else if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execve("./cgi/bin/roulette.cgi", argv, envp);
        printError();
        exit(EXIT_FAILURE);
    }
    else {
        close(fd[1]);

        char buffer[100];
        int bytes;

        while((bytes = read(fd[0], buffer, 100)) > 0)
            this->_stdout.append(buffer, bytes);

        close(fd[0]);

        int status;
        waitpid(pid, &status, 0);
        generateResponse(request, response);
        return WEXITSTATUS(status);
    }
}

int Cgi::execFromPost(HttpRequest &request, HttpResponse &response) {
    (void)request;
    (void)response;
    //TODO
    return EXIT_SUCCESS;
}

void Cgi::generateResponse(HttpRequest &request, HttpResponse &response) {
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "text/html\n");
    response.setBody(this->_stdout);
}

void Cgi::execute(HttpRequest &request, HttpResponse &response) {

    if(request.getMethod() == "GET")
        if(execFromGet(request, response) == EXIT_FAILURE)
            generateErrorMsg(request, response);
    if(request.getMethod() == "POST")
        if(execFromPost(request, response) == EXIT_FAILURE)
            generateErrorMsg(request, response);
}

Cgi::Cgi(HttpRequest &request, HttpResponse &response) {
    this->execute(request, response);
}

Cgi::~Cgi() {

}
