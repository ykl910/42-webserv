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
	int execFromGet(HttpRequest &request);
	int execFromPost(HttpRequest &request);
	void generateErrorMsg(HttpRequest &request, HttpResponse &response);
	void generateResponse(HttpRequest &request, HttpResponse &response);
	std::string extractQuery(HttpRequest &request);
	std::vector<char*> createEnv(HttpRequest &Request);
	void extractOutput(int *fd);

	std::string _stdout;
};
