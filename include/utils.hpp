#include <string>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

std::string itos(int value);
std::vector<std::string>split(std::string string, std::string &boundary);
void writeUserInfo(HttpRequest &request, HttpResponse &response);