#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "utils.hpp"
#include "Error.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <string>

class Cgi {
public:
    Cgi(HttpRequest &request, HttpResponse &response);
    ~Cgi();

private:
    void createEnvp(HttpRequest &request);
    void createArgv(HttpRequest &request);
    void execute(HttpRequest &request, HttpResponse &response);
    int execFromGet();
    int execFromPost(HttpRequest &request);
    void createEnvpStr(std::vector<char*>&envp);
    void createArgvStr(std::vector<char*>&argv);
    std::string extractQuery(HttpRequest &request);
    void extractOutput(int *fd);
    void generateErrorMsg(HttpRequest &request, HttpResponse &response);
    void generateResponse(HttpRequest &request, HttpResponse &response);

    std::string _stdout;
    std::vector<std::string>_envp;
    std::vector<std::string>_argv;
};
