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

#define ON 1

bool    HttpResponse::isDirectory()
{
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

    locationMapIterator it =  _server.locationMap.find(_request.path);
    if (it != _server.locationMap.end()) {
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

    struct dirent *dirInfo = readdir(indexDir);
    if (!dirInfo) {
        closedir(indexDir);
        std::cerr << BOLD RED << "ERROR readdir\n" << DEFAULT;
    }
}
