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

int createFile(std::string filename) {

    std::string filepath = DIRPATH + filename;
    int fd = open(filepath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if(fd == -1)
        printError();
    return fd;
}

int storeTitle(std::string title, int threadNb) {

    std::cout << GREEN <<"Title:" << title << DEFAULT <<std::endl;
    std::string filename = itos(threadNb) + "_title.txt";
    int fd = createFile(filename);
    if(fd == -1)
        return -1;
    size_t startPos = title.find("\r\n\r\n") + 4;
    size_t endPos = title.size() - 2;

    std::string txt = title.substr(startPos, endPos - startPos);

    write(fd, txt.c_str(), txt.size());
    close(fd);
    return 0;
}

int storeText(std::string body, int threadNb) {

    std::cout << RED << "Texte: " << body << DEFAULT << std::endl;
    std::string filename = itos(threadNb) + "_body.txt";
    int fd = createFile(filename);
    if(fd == -1)
        return -1;
    size_t startPos = body.find("\r\n\r\n") + 4;
    size_t endPos = body.size() - 2;

    std::string txt = body.substr(startPos, endPos - startPos);

    write(fd, txt.c_str(), txt.size());
    close(fd);
    return 0;
}

int storeImg(std::string img, int threadNb) {
    (void)threadNb;
    std::cout << YELLOW << "Img:" << img << DEFAULT << std::endl;
    std::string filename = itos(threadNb) + "_img.jpg";
    int fd = createFile(filename);
    if(fd == -1)
        return -1;
    size_t startPos = img.find("\r\n\r\n") + 4;
    size_t endPos = img.size() - 2;

    std::string txt = img.substr(startPos, endPos - startPos);

    write(fd, txt.c_str(), txt.size());
    close(fd);
    return 0;
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
        {
            if(storeTitle(line, threadNb) == -1)
                return -1;
        }
        if(line.find("Content-Disposition: form-data; name=\"body\"")!= std::string::npos)
        {
            if(storeText(line, threadNb) == -1)
                return -1;
        }
        if(line.find("Content-Disposition: form-data; name=\"uploadFile\"")!= std::string::npos)
        {
            if(storeImg(line, threadNb) == -1)
                return -1;
        }
    }
    if(threadNb + 1 == 2147483647)
        threadNb = 0;
    else
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
        else
            std::cout << "ok store thread" << std::endl; //TODO: send appropriate response ok
    }
    else
    {
        //TODO: BAD REQUEST and send appropriate response
        std::cout << "not a multipart/form-data" << std::endl;
    }
}
