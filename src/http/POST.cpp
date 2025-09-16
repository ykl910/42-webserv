#include "../../include/textFormatting.hpp"
#include "../../include/HttpResponse.hpp"
#include "../../include/Cookies.hpp"
#include "../../include/WebServ.hpp"

# define DIRPATH "www/post42.net/threads/"

std::string HttpResponse::getBoundary(std::string &line)
{
    size_t boundaryStart = line.find("=");
    size_t boundaryEnd = line.length();
    if(boundaryStart != std::string::npos && boundaryEnd != std::string::npos)
        return line.substr(boundaryStart + 1, boundaryEnd);
    return NULL;
}

int HttpResponse::createFd(std::string dir, std::string filename)
{
    std::string fullpath = dir + "/" + filename;
    int fd = open(fullpath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if(fd == -1)
        printError();
    return fd;
}

int HttpResponse::createFile(std::string &content, std::string &dirPath, int id)
{
    std::string filename;
    size_t titleStart = content.find("name=") + 6;
    size_t titleEnd = content.find("\"", titleStart);
    if(content.find("Content-Type: image") != std::string::npos)
        filename = content.substr(titleStart, titleEnd - titleStart) + itos(id) + ".png";
    else
        filename = content.substr(titleStart, titleEnd - titleStart) + itos(id) + ".txt";
    std::cout << ITALIC RED << "filename: " << filename << DEFAULT << std::endl;
    int fd = createFd(dirPath, filename);
    if(fd == -1)
        return -1;
    size_t contentStartPos = content.find("\r\n\r\n") + 4;
    size_t contentEndPos = content.size() - 2;
    std::string txt = content.substr(contentStartPos, contentEndPos - contentStartPos);
    write(fd, txt.c_str(), txt.size());
    close(fd);
    return 0;
}

bool HttpResponse::directoryExist(std::string dirPath)
{
    DIR *directory = opendir(dirPath.c_str());
    if(directory)
    {
        closedir(directory);
        return true;
    }
    return false;
}

int HttpResponse::createDirectory(std::string dirPath)
{
    if (mkdir(dirPath.c_str(), 0755) == -1) {
        printError();
        return -1;
    }
    return 0;
}

int HttpResponse::downloadFiles(HttpRequest &request, std::string boundary)
{
    static int id = 0;
    std::string dirPath = _server.rootLocation + "/downloads";

    if (!directoryExist(dirPath)) {
        if(createDirectory(dirPath) == -1)
            return -1;
    }

    std::vector<std::string> tokens = split(request.getBody(), boundary);
    std::vector<std::string>::iterator tokenIt;

    for (tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
        std::string line = *tokenIt;
        std::cout << BOLD RED << "LINE: " << line.substr(0,500) << DEFAULT << std::endl;
        if (line.find("Content-Disposition: form-data") != std::string::npos) {
            if (createFile(line, dirPath, id) == -1)
              return -1;
        }
    }
    id++;
    if(id == INT_MAX)
        id = 0;
    return 0;
}

void HttpResponse::buildResponse(int code, std::string msg)
{
    setStatusLine(getRequestAttr().httpVersion, code, msg);
    setHeaders("Content-Type", "text/html");
    std::string path;
    if (code == 201)
        setStatusLine(_request.httpVersion, 201, "ok");
    else if (code == 400)
        path = _server.error_page.err_400;
    else if (code == 500)
        path = _server.error_page.err_500;
    else
        path = _server.error_page.err_404;
    std::ifstream file(path.c_str());
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string body = buffer.str();
    setHeaders("Content-Length", itos(body.size()));
    setBody(body);
}

inline bool HttpResponse::isFormData(std::string &contentType) const
{
    std::cout << BOLD RED << "CONTENT TYPE: " << contentType << DEFAULT << std::endl;

    return (contentType.find("multipart/form-data") != std::string::npos);
}

void HttpResponse::handlePOST(HttpRequest& request)
{
    std::map<std::string, std::string> headers = request.getHeaders();

    if ((isFormData(headers["Content-Type"])))
    {
        std::cout << BOLD GREEN << "IS FORM DATA" << DEFAULT << std::endl;
        std::string boundary = getBoundary(headers["Content-Type"]);
        if (downloadFiles(request, boundary) == -1)
            buildResponse(500, "Internal error");
        else
            buildResponse(201, "Created");
    }
    else
        buildResponse(404, "Not found");
}
