#include "../../include/HttpResponse.hpp"

bool    HttpResponse::isDirectory()
{
    std::cout << BOLD WHITE << _request.path << DEFAULT << std::endl;
    DIR *dir = opendir(_request.path.c_str());
    if (!dir)
        return false;
    closedir(dir);
    return true;
}

bool    HttpResponse::isAutoIndex()
{
    if (_isAutoIndex == true)
        return true;
    return false;
}


void    HttpResponse::buildIndex()
{
    DIR *indexDir= opendir(_request.path.c_str());
    if (!indexDir)
        std::cerr << BOLD RED << "ERROR opendir\n" << DEFAULT;
    else {

        std::string         name;
        std::string         fullPath;
        std::ostringstream  output;

        output << "<html>\n"
               << "<head><title>Index of "
               << _request.path
               << "</title></head>\n"
               << "<body>\n"
               << "<h1>Index of " << _request.path << "</h1>\n"
               << "<hr><ul>\n";

        output << "<li><a href=\"../\">../</a></li>\n";

        while (true) {
            struct dirent *dirInfo = readdir(indexDir);
            if (!dirInfo) {
                std::cerr << BOLD RED << "ERROR readdir\n" << DEFAULT;
                break;
            }
            name = dirInfo->d_name;
            std::cout << name << std::endl;
            if (name != "." && name != "..") {
                fullPath = _request.path + "/" + name;
                struct stat fileStat;
                if (stat(fullPath.c_str(), &fileStat) == -1)
                    std::cerr << BOLD RED << "Can't open file\n" << DEFAULT;
                else
                {
                    if (S_ISDIR(fileStat.st_mode))
                        name += "/";
                    output
                    << "<li><a href=\""
                    << name << "\">" << name << "</a></li>\n";
                }
            }
            std::cout << BOLD WHITE << fullPath << DEFAULT << '\n';
        }
        output << "</ul><hr></body></html>";
        int len = output.str().length();
        setBody(output.str());
        setHeaders("Content-Type", "text/html");
        setHeaders("Content-Length", itos(len));
        setStatusLine(_request.httpVersion, 200, "ok");
        closedir(indexDir);
    }
}
