#include "../../include/textFormatting.hpp"
#include "../../include/HttpResponse.hpp"
#include "../../include/Cookies.hpp"
#include "../../include/WebServ.hpp"

# define DIRPATH "www/post42.net/threads/"

std::string getBoundary(std::string &line)
{
    size_t boundaryStart = line.find("=");
    size_t boundaryEnd = line.length();
    if(boundaryStart != std::string::npos && boundaryEnd != std::string::npos)
        return line.substr(boundaryStart + 1, boundaryEnd);
    return NULL;
}

int createFile(std::string dir, std::string filename)
{
    std::string fullpath = dir + filename;
    int fd = open(fullpath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if(fd == -1)
        printError();
    return fd;
}

int setThread(std::string img, int threadNb, const std::string& threadType)
{
    std::string filename;
    filename = itos(threadNb) + threadType;
    int fd = createFile(DIRPATH, filename);
    if(fd == -1)
        return -1;
    size_t startPos = img.find("\r\n\r\n") + 4;
    size_t endPos = img.size() - 2;

    std::string txt = img.substr(startPos, endPos - startPos);

    write(fd, txt.c_str(), txt.size());
    close(fd);
    return 0;
}

bool directoryExist()
{
    DIR *directory = opendir(DIRPATH);
    if(directory)
    {
        closedir(directory);
        return true;
    }
    return false;
}

int createDirectory()
{
    if (mkdir(DIRPATH, 0755) == -1) {
        printError();
        return -1;
    }
    return 0;
}

int storeThread(HttpRequest &request, std::string boundary)
{
    static int threadNb = 0;

    if (!directoryExist()) {
        if(createDirectory() == -1)
            return -1;
    }

    std::vector<std::string> tokens = split(request.getBody(), boundary);
    std::vector<std::string>::iterator tokenIt;

    for (tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
        std::string line = *tokenIt;
        if (line.find("Content-Disposition: form-data; name=\"title\"") != std::string::npos) {
            if (setThread(line, threadNb, "_title.txt") == -1)
                return -1;
        }
        if (line.find("Content-Disposition: form-data; name=\"body\"") != std::string::npos) {
            if (setThread(line, threadNb, "_body.txt") == -1)
                return -1;
        }
        if (line.find("Content-Disposition: form-data; name=\"uploadFile\"") != std::string::npos) {
            if (setThread(line, threadNb, "_img.jpg") == -1)
                return -1;
        }
    }
    if (threadNb + 1 == 2147483647)
        threadNb = 0;
    else
        threadNb++;
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
        if (storeThread(request, boundary) == -1)
            buildResponse(500, "Internal error");
        else
            buildResponse(201, "Created");
    }
    else
        buildResponse(404, "Not found");
}
