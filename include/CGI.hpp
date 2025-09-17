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
#include <sys/time.h>

class HttpResponse;
class HttpManager;

class Cgi {
public:
    Cgi(HttpRequest &request, HttpResponse &response);
    ~Cgi();

private:
    void createEnvp(HttpRequest &request, HttpResponse &response);
    void createArgv(HttpResponse &response);
    void execute(HttpRequest &request, HttpResponse &response);
    int execFromGet();
    int execFromPost(HttpRequest &request);
    void createEnvpStr(std::vector<char*>&envp);
    void createArgvStr(std::vector<char*>&argv);
    std::string extractQuery(HttpResponse &response);
    void extractOutput(int *fd);
    void generateErrorMsg(HttpResponse &response, int statusCode);
    void generateResponse(HttpResponse &response);
    void watchdog(pid_t pid, int &status);
    long getTimeStamp(void);
    std::string _stdout;
    std::vector<std::string>_envp;
    std::vector<std::string>_argv;
};
