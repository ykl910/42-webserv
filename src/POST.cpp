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

    std::string filename;
    filename = itos(threadNb) + "_img.jpg";

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

void buildResponse(HttpRequest& request, HttpResponse& response, int code, std::string msg) {

    response.setStatusLine(request.getHttpVersion(), code, msg);
    response.setHeaders("Content-Type", "text/html");
    std::string path;
    if(code == 201)
        path = "./website/html/uploadSucces.html";
    else if(code == 400)
        path = "./website/html/400.html";
    else if(code == 500)
        path = "./website/html/500.html";
    else
        path = "./website/html/404.html";
    std::ifstream file(path.c_str());
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string body = buffer.str();
    response.setHeaders("Content-Length", itos(body.size()));
    response.setBody(body);
}

void handlePost(HttpRequest& request, HttpResponse& response) {

    std::map<std::string, std::string> headers = request.getHeaders();
    std::string contentType = getContentType(headers["Content-Type"]);
    std::string boundary = getBoundary(headers["Content-Type"]);

    if((!contentType.empty() && contentType == "multipart/form-data") && !boundary.empty())
    {
        if(storeThread(request, boundary) == -1)
            buildResponse(request, response, 500, "Internal error");
        else
            buildResponse(request, response, 201, "Created");
    }
    else
        buildResponse(request, response, 400, "Bad Request");
}
