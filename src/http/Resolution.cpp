
#include "../../include/HttpResponse.hpp"

bool    HttpResponse::isImage()
{
    return _extension == "jpg" || _extension == "jpeg" || _extension == "png"
        || _extension == "gif" || _extension == "bmp" || _extension == "tif"
        || _extension == "tiff" || _extension == "ico" || _extension == "webp"
        || _extension == "avif" || _extension == "svg" || _extension == "apng"
        || _extension == "heic" || _extension == "heif";
}

void    HttpResponse::solvePath()
{
    std::string fullPath;
    size_t dot_pos = _request.path.find_last_of(".");
    if (dot_pos != std::string::npos)
        _extension = _request.path.substr(dot_pos + 1);

    for (size_t i = 0; i < _server.location.size(); ++i) {

        if (_request.path.find(_server.location[i].path) == 0) {
            _isAutoIndex = false;
            _isIndex = false;
            _allowedMethod = _server.location[i].method;

            if (_request.path == "" || _request.path == _server.location[i].path
                || _request.path == _server.location[i].path + "/") {
                _isIndex = true;
                if (_server.location[i].index != "n")
                    fullPath = _server.location[i].root
                        + "/" + _server.location[i].index;
                else {
                    if (_server.location[i].autoindex == ON)
                        _isAutoIndex = true;
                    else
                        _isAutoIndex = false;
                    fullPath = _server.location[i].root;
                }

            } else {
                std::string root = "";
                if (_server.location[i].path == "/")
                    root = "/";

                std::string locPath = _server.location[i].path;
                std::string relativePath = root
                    + _request.path.substr(locPath.length());
                fullPath = _server.location[i].root + relativePath;
            }
        }
    }
    _request.path = fullPath;
    _root = _server.rootLocation;
}
