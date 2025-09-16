#include "../../include/HttpResponse.hpp"

bool    HttpResponse::canAccessFile(std::string& fullPath)
{
    struct stat fileStat;

    if (stat(fullPath.c_str(), &fileStat) != 0) {
        std::cout << BOLD RED << "Can't find requested file\n" << DEFAULT;
        return false;
    }

    if (!(fileStat.st_mode & S_IRUSR)) {
        std::cout << BOLD RED << "Don't have read permissions\n" << DEFAULT;
        return false;
    }

    return true;
}

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
    if (!isDirectory())
        return false;

    locationMapIterator it =  _server.locMap.find(_request.path);
    if (it != _server.locMap.end()) {
        if (it->second.autoindex == ON)
            return true;
    }
    return false;
}

void    HttpResponse::buildIndex()
{
    DIR *indexDir= opendir(_request.path.c_str());
    if (!indexDir)
        std::cerr << BOLD RED << "ERROR opendir\n" << DEFAULT;
    else {
        while (true) {
            struct dirent *dirInfo = readdir(indexDir);
            if (!dirInfo) {
                std::cerr << BOLD RED << "ERROR readdir\n" << DEFAULT;
                break;
            }

            struct stat fileStat;
            if (stat(_request.path.c_str(), &fileStat) == -1)
                std::cerr << BOLD RED << "Can't open file\n" << DEFAULT;

            std::cout
            << dirInfo->d_name << '\n';
        }
        closedir(indexDir);
    }
}
