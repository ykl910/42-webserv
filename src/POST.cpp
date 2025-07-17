#include "../include/POST.hpp"
#include "../include/textFormatting.hpp"
#include "../include/WebServ.hpp"

# define DIRPATH "website/threads/"

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

void storeTitle(std::string title, int threadNb) {

    std::cout << GREEN <<"Title:" << title << DEFAULT <<std::endl;
    std::string filename = "title" + itos(threadNb);
}

void storeText(std::string body, int threadNb) {
    (void)threadNb;
    std::cout << RED << "Texte: " << body << DEFAULT << std::endl;
}

void storeImg(std::string img, int threadNb) {
    (void)threadNb;
    std::cout << YELLOW << "Img:" << img << DEFAULT << std::endl;
}

bool directoryExist() {

    DIR *directory = opendir(DIRPATH);
    if(directory)
    {
        closedir(directory);
        return true;
    }
    return false;
}

int createDirectory() {

    if(mkdir(DIRPATH, 0755) == -1)
    {
        printError();
        return -1;
    }
    return 0;
}

int storeThread(HttpRequest &request, std::string boundary) {

    static int threadNb = 0;

    if(!directoryExist())
    {
        if(createDirectory() == -1)
            return -1;
    }

    std::vector<std::string> tokens = split(request.getBody(), boundary);
    std::vector<std::string>::iterator tokenIt;

    for(tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt)
    {
       std::string line = *tokenIt;
       if(line.find("Content-Disposition: form-data; name=\"title\"") != std::string::npos)
           storeTitle(line, threadNb);
       if(line.find("Content-Disposition: form-data; name=\"body\"")!= std::string::npos)
           storeText(line, threadNb);
       if(line.find("Content-Disposition: form-data; name=\"uploadFile\"")!= std::string::npos)
           storeImg(line, threadNb);
    }
    threadNb++;
    return 0;
}

void handlePost(HttpRequest& request, HttpResponse& response) {

    (void)response;
    std::map<std::string, std::string> headers = request.getHeaders();
    std::string contentType = getContentType(headers["Content-Type"]);
    std::string boundary = getBoundary(headers["Content-Type"]);

    if(contentType.empty() || boundary.empty())
    {
        //TODO: BAD REQUEST and send appropriate response
        std::cout << "contentType.empty() || boundary.empty()" << std::endl;
    }
    else if(contentType == "multipart/form-data" && !boundary.empty())
    {
        if(storeThread(request, boundary) == -1)
            std::cout << "error store thread" << std::endl; //TODO: send appropriate response error
        std::cout << "ok store thread" << std::endl; //TODO: send appropriate response ok
    }
    else
    {
        //TODO: BAD REQUEST and send appropriate response
        std::cout << "not a multipart/form-data" << std::endl;
    }
}
