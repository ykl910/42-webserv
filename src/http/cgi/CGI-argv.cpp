#include "../../../include/CGI.hpp"

void Cgi::createArgv(HttpResponse &response)
{
    std::string path = response.getRequestAttr().path;
    size_t questionMarkPos = path.rfind("?");
    size_t extentionDotPos = path.rfind(".");

    if (questionMarkPos == std::string::npos
        || questionMarkPos < extentionDotPos)
        _argv.push_back(path);
    else
        _argv.push_back(path.substr(0, questionMarkPos));
}

void Cgi::createArgvStr(std::vector<char*> &argv)
{
    for (size_t i = 0; i < _argv.size(); ++i)
        argv.push_back(const_cast<char*>(_argv[i].c_str()));
    argv.push_back(NULL);
}
