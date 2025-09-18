#include "../../../include/CGI.hpp"

std::string Cgi::extractQuery(HttpResponse &response)
{
    std::string header = response.getRequestAttr().path;
    size_t start = header.rfind('?') + 1;
    std::string var = header.substr(start, header.size() - start);

    return var;
}

void Cgi::createEnvp(HttpRequest &request, HttpResponse &response)
{
    std::string method = response.getRequestAttr().method;

    _envp.push_back("REQUEST_METHOD=" + method);
    _envp.push_back("SCRIPT_NAME=" + response.getRequestAttr().path);
    if (method == "GET")
        _envp.push_back("QUERY_STRING=" + extractQuery(response));

    else {
        std::map<std::string, std::string> header = request.getHeaders();

        _envp.push_back("CONTENT_LENGTH=" + header["Content-Length"]);
        _envp.push_back("CONTENT_TYPE=" + header["Content-Type"]);
    }
}

void Cgi::createEnvpStr(std::vector<char*> &envp)
{
    for (size_t i = 0; i < _envp.size(); ++i)
        envp.push_back(const_cast<char*>(_envp[i].c_str()));
    envp.push_back(NULL);
}
