#include "../../../include/textFormatting.hpp"
#include "../../../include/HttpResponse.hpp"
#include "../../../include/WebServ.hpp"

void HttpResponse::handleDELETE()
{
    std::string body;
    if (access(_request.path.c_str(), F_OK) != 0) {
        body = "Not Found";
        setStatusLine(_request.httpVersion, 404, body);

    } else if (access(_request.path.c_str(), W_OK) != 0) {
        body = "Forbidden";
        setStatusLine(_request.httpVersion, 403, body);

    } else if (unlink(_request.path.c_str()) == 0) {
        body = "File deleted successfully";
        setStatusLine(_request.httpVersion, 200, body);

    } else {
        body = "Internal Server Error";
        setStatusLine(_request.httpVersion, 500, body);
    }

    setHeaders("Content-Type", "text/plain");
    setHeaders("Content-Length", itos(body.length()));
    setBody(body);
}
