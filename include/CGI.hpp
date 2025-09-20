#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "utils.hpp"
#include "Error.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <string>

class HttpResponse;
class HttpManager;

class Cgi {
public:
    void createArgv(HttpResponse &response);
    void createEnvpStr(std::vector<char*>&envp);
    void createArgvStr(std::vector<char*>&argv);
    void createEnvp(HttpRequest &request, HttpResponse &response);

    int execFromGet();
    void extractOutput(int *fd);
    int execFromPost(HttpRequest &request);
    void execute(HttpRequest &request, HttpResponse &response);

    long getTimeStamp(void);
    void watchdog(pid_t pid, int &status);
    std::string extractQuery(HttpResponse &response);


    std::string getBinDir(std::string binPath);
    std::string getBinName(std::string binPath);

    void generateResponse(HttpResponse &response);
    void generateErrorMsg(HttpResponse &response, int statusCode);

    Cgi(HttpRequest &request, HttpResponse &response);
    ~Cgi();

private:
    std::string _stdout;
    std::vector<std::string>_envp;
    std::vector<std::string>_argv;
};
