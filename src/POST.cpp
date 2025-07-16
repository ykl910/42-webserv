#include "../include/POST.hpp"

std::string getBoundary(std::string &line) {

    size_t boundaryStart = line.find("=");
    size_t boundaryEnd = line.length();
    if(boundaryStart != std::string::npos && boundaryEnd != std::string::npos)
        return line.substr(boundaryStart + 1, boundaryEnd);
    return NULL;
}

std::string getContentType(std::string &line) {

    size_t typeEnd = line.find(";");
    if(typeEnd != std::string::npos)
        return line.substr(0, typeEnd);
    return NULL;
}

int getContentLength(std::string &line) {

    return std::atoi(line.c_str());
}

void handlePost(HttpRequest& request, HttpResponse& response) {

    std::map<std::string, std::string> headers = request.getHeaders();
    std::string contentType = getContentType(headers["Content-Type"]);
    std::string boundary = getBoundary(headers["Content-Type"]);
    int contentLength = getContentLength(headers["Content-Length"]);
    if(contentType.empty() || contentLength == 0 || boundary.empty())
        //TODO: BAD REQUEST

    std::string threadTitle;
    std::string threadText;
    std::string binaryImg;
    (void)response;
}
