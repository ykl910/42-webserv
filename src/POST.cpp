#include "../include/POST.hpp"
#include "../include/textFormatting.hpp"

std::vector<std::string>split(std::string string, std::string &boundary) {

    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end;

    while((end = string.find(boundary, start)) != std::string::npos)
    {
        tokens.push_back(string.substr(start, end - start));
        start = end + boundary.length();
    }
    tokens.push_back(string.substr(start));
    return tokens;
}

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


void storeTitle(std::string body) {

    std::cout << GREEN <<"Title:" << body << DEFAULT <<std::endl;
}

void storeText(std::string body) {

    std::cout << RED << "Texte: " << body << DEFAULT << std::endl;
}

void storeImg(std::string body) {

    std::cout << YELLOW << "Img:" << body << DEFAULT << std::endl;
}

void handlePost(HttpRequest& request, HttpResponse& response) {


    (void)response;
    std::map<std::string, std::string> headers = request.getHeaders();
    std::string contentType = getContentType(headers["Content-Type"]);
    std::string boundary = getBoundary(headers["Content-Type"]);

    if(contentType.empty() || boundary.empty())
    {
        //TODO: BAD REQUEST
        std::cout << "ouinoin" << std::endl;
    }

    std::vector<std::string> tokens = split(request.getBody(), boundary);
    std::vector<std::string>::iterator tokenIt;

    for(tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt)
    {
       std::string line = *tokenIt;
       if(line.find("Content-Disposition: form-data; name=\"title\"") != std::string::npos)
           storeTitle(line);
       if(line.find("Content-Disposition: form-data; name=\"body\"")!= std::string::npos)
           storeText(line);
       if(line.find("Content-Disposition: form-data; name=\"uploadFile\"")!= std::string::npos)
           storeImg(line);
    }
}
