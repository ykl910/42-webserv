
#include "../../include/HttpResponse.hpp"

bool    HttpResponse::isImage()
{
    return _extension == "jpg" || _extension == "jpeg" || _extension == "png"
        || _extension == "gif" || _extension == "bmp" || _extension == "tif"
        || _extension == "tiff" || _extension == "ico" || _extension == "webp"
        || _extension == "avif" || _extension == "svg" || _extension == "apng"
        || _extension == "heic" || _extension == "heif";
}

bool    HttpResponse::isAudio()
{
    return _extension == "mp3" || _extension == "wav" || _extension == "ogg"
        || _extension == "oga" || _extension == "m4a" || _extension == "aac"
        || _extension == "flac" || _extension == "opus";
}

bool    hostFound(std::string& input)
{
    (void)input;
    return true;
}

// stat
// opendir
// chdir
// readdir
// closedir

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

void    HttpResponse::solveMimeType()
{
    return;
}

void    HttpResponse::solvePath()
{
    std::string fullPath;
    size_t dot_pos = _request.path.find_last_of(".");
    if (dot_pos != std::string::npos)
        _extension = _request.path.substr(dot_pos + 1);

    for (size_t i = 0; i < _server.location.size(); ++i)
    {
        // std::cout << "Request PATH: " << _request.path << "\n";
        // std::cout << "Server location path: " << i << _server.location[i].path << "\n";
        if (_request.path.find(_server.location[i].path) == 0)
        {
            // std::cout << "Request PATH: " << _request.path << "\n";
            // std::cout << "Server location path: " <<  _server.location[i].path << "\n";
            // std::cout << "Server location root: " <<  _server.location[i].root << "\n";
            if (_request.path == "" || _request.path == _server.location[i].path
                || _request.path == _server.location[i].path + "/")
                fullPath = _server.location[i].root + "/" + _server.location[i].index;
            else
            {
                std::string root = "";
                if (_server.location[i].path == "/")
                    root = "/";
                std::string locPath = _server.location[i].path;
                std::string relativePath = root + _request.path.substr(locPath.length());
                fullPath = _server.location[i].root + relativePath;
            }
        }
    }
    _request.path = fullPath;
    std::cout << BOLD BLUE << _request.path << std::endl << DEFAULT;
}

void    HttpResponse::buildResponse(void)
{
    solvePath();
    solveMimeType();
}