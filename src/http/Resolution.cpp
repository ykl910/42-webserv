
#include "../../include/HttpResponse.hpp"

bool    HttpResponse::isImage()
{
    return _extension == "jpg" || _extension == "jpeg" || _extension == "png"
        || _extension == "gif" || _extension == "bmp" || _extension == "tif"
        || _extension == "tiff" || _extension == "ico" || _extension == "webp"
        || _extension == "avif" || _extension == "svg" || _extension == "apng"
        || _extension == "heic" || _extension == "heif";
}

bool    hostFound(std::string& input)
{
    (void)input;
    return true;
}

void    HttpResponse::solvePath()
{
    std::string fullPath;
    size_t dot_pos = _request.path.find_last_of(".");
    if (dot_pos != std::string::npos)
        _extension = _request.path.substr(dot_pos + 1);

    for (size_t i = 0; i < _server.location.size(); ++i)
    {
        if (_request.path.find(_server.location[i].path) == 0)
        {
            _allowedMethod = _server.location[i].method;
            if (_request.path == "" || _request.path == _server.location[i].path
                || _request.path == _server.location[i].path + "/")
                fullPath = _server.location[i].root + "/" + _server.location[i].index;
            else
            {
                std::string root = "";
                if (_server.location[i].path == "/")
                    root = "/";
                std::string locPath = _server.location[i].path;
                std::cout << BOLD RED << "loc path: " << _server.location[i].path << DEFAULT << '\n';

                std::string relativePath = root + _request.path.substr(locPath.length());
                std::cout << BOLD RED << "rel path: " << relativePath << DEFAULT << '\n';

                fullPath = _server.location[i].root + relativePath;
            }
        }
    }
    if (isAutoIndex())
        std::cout << BOLD RED << "full path: " << _request.path << DEFAULT << '\n';
    else {
        std::cout << BOLD YELLOW << "full path: " << fullPath << DEFAULT << '\n';
        _request.path = fullPath;
    }
    _root = _server.rootLocation;
}
