#pragma once
#include "Error.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class HttpResponse;
class HttpRequest;

class Cgi
{
  public:
	Cgi(HttpRequest &request, HttpResponse &response);
	~Cgi();
	void execute(HttpRequest &request, HttpResponse &response);

  private:
	int execFromGet(HttpRequest &request, HttpResponse &response);
	int execFromPost(HttpRequest &request, HttpResponse &response);
	void generateErrorMsg(HttpRequest &request, HttpResponse &response);
	void generateResponse(HttpRequest &HttpRequest, HttpResponse &response);
	std::string _stdout;
	std::vector<std::string> _rawEnvp;
	std::vector<char *> _envp;
};
