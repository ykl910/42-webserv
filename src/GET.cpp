#include "../include/GET.hpp"

std::string itos(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

bool handleHtml(HttpRequest& request, HttpResponse& response, std::string path, std::stringstream *buffer) {
    std::string fullPath = "./website/html" + path;
    std::ifstream file(fullPath.c_str());
    if (!file.is_open())
    {
        response.setStatusLine(request.getHttpVersion(), 404, "Not Found");
        return false;
    }
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "text/html");
    response.setHeaders("Content-Length", itos(body.length()));
    response.setBody(buffer->str());
    return true;
}

bool handleCss(HttpRequest& request, HttpResponse& response, std::string path, std::stringstream *buffer) {
    std::string fullPath = "./website/html" + path;
    std::ifstream file(fullPath.c_str());
    if (!file.is_open())
    {
        response.setStatusLine(request.getHttpVersion(), 404, "Not found");
        return false;
    }
    *buffer << file.rdbuf();
    std::string body = buffer->str();
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "text/css");
    response.setHeaders("Content-Length", itos(body.length()));
    response.setBody(buffer->str());
    return true;
}

bool handlePng(HttpRequest& request, HttpResponse& response, std::string path) {
    std::string fullPath = "./website" + path;
    std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        response.setStatusLine(request.getHttpVersion(), 404, "Not Found");
        return false;
    }
    std::vector<char> data;
    char temp[4096];
    while (file.read(temp, sizeof(temp))) {
        data.insert(data.end(), temp, temp + file.gcount());
    }
    if (file.gcount() > 0) {
        data.insert(data.end(), temp, temp + file.gcount());
    }
    response.setStatusLine(request.getHttpVersion(), 200, "OK");
    response.setHeaders("Content-Type", "image/png");
    response.setHeaders("Content-Length", itos(data.size()));
    response.setBody(std::string(&data[0], data.size()));
    return true;
}

// bool handlePng(HttpRequest& request, HttpResponse& response, std::string path) {
//     std::string fullPath = "./website" + path;
    
//     // Try C-style file handling instead of ifstream
//     FILE* file = fopen(fullPath.c_str(), "rb");
//     if (!file) {
//         response.setStatusLine(request.getHttpVersion(), 404, "Not Found");
//         return false;
//     }
    
//     // Get file size
//     fseek(file, 0, SEEK_END);
//     unsigned long fileSize = ftell(file);
//     fseek(file, 0, SEEK_SET);
    
//     // Read file in chunks to handle large files better
//     std::vector<char> buffer;
//     buffer.reserve(fileSize);
    
//     char chunk[8192];
//     size_t bytesRead;
//     while ((bytesRead = fread(chunk, 1, sizeof(chunk), file)) > 0) {
//         buffer.insert(buffer.end(), chunk, chunk + bytesRead);
//     }
//     fclose(file);
    
//     if (buffer.size() != fileSize) {
//         response.setStatusLine(request.getHttpVersion(), 500, "Internal Server Error");
//         return false;
//     }
    
//     response.setStatusLine(request.getHttpVersion(), 200, "OK");
//     response.setHeaders("Content-Type", "image/png");
//     response.setHeaders("Content-Length", itos(fileSize));
    
//     // Additional headers for large files
//     response.setHeaders("Accept-Ranges", "bytes");
//     response.setHeaders("Cache-Control", "public, max-age=3600");
    
//     std::string binaryString(&buffer[0], buffer.size());
//     response.setBody(binaryString);
    
//     return true;
// }

void handleGet(HttpRequest& request, HttpResponse& response) {
    std::string root = "./website";
    std::string path = request.getPath();
    if (path == "" || path == "/")
        path = "/index.html";
    std::string extension;
    size_t dot_pos = path.find_last_of(".");
    if (dot_pos != std::string::npos) {
        extension = path.substr(dot_pos + 1);
    }
    bool success = false;
    std::stringstream buffer;
    if (extension == "html" || extension == "htm") {
        success = handleHtml(request, response, path, &buffer);
    } else if (extension == "css") {
        success = handleCss(request, response, path, &buffer);
    } else if (extension == "png") {
        success = handlePng(request, response, path);
    }
}
