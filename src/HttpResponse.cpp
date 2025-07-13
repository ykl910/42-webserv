#include "../include/HttpResponse.hpp"
#include "../include/ResponseHandler.hpp"

HttpResponse::HttpResponse(HttpRequest &request){
    (void)request;
}

void HttpResponse::setStatusLine(const std::string version, int code, const std::string &reason){
    std::ostringstream oss;
    oss << code;
    _statusLine = version + oss.str() + reason;
}

void HttpResponse::setHeaders(const std::string &key, const std::string &value){
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string &body){
    _body = body;
}

std::string HttpResponse::getResponse() const {
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html; charset=UTF-8\r\n"
           "\r\n"
           "<!DOCTYPE html>\n"
           "<html lang=\"fr\">\n"
           "<head>\n"
           "    <link rel=\"stylesheet\" href=\"style.css\">\n"
           "    <meta charset=\"UTF-8\">\n"
           "    <title>Les éveillés</title>\n"
           "</head>\n"
           "<body>\n"
           "    <h1>Sortez de la matrice !</h1>\n"
           "\n"
           "    <h2>Préambule</h2>\n"
           "    <img src=\"../images/hackers.png\" alt=\"Préambule\">\n"
           "    <p>\n"
           "        Depuis des siècles, les gouvernements et la communauté scientifique vous mentent.<br>\n"
           "        Ce que vous pensiez être des oiseaux ? Des drones.<br>\n"
           "        Ce que vous croyez être la Finlande ? Une invention de Google Maps.<br>\n"
           "        À qui profite réellement la vente de Dragibus ? À un réseau criminel international, évidemment.<br>\n"
           "        Je vous propose ici une liste non exhaustive de ces vérités cachées, soigneusement ignorées par les médias...\n"
           "    </p>\n"
           "\n"
           "    <h2>Les oiseaux de la CIA :</h2>\n"
           "    <img src=\"../images/bird.png\" alt=\"Drones déguisés en oiseaux\">\n"
           "    <ul>\n"
           "        <li><a href=\"theories/oiseaux.html\">Les oiseaux sont des drones</a></li>\n"
           "    </ul>\n"
           "\n"
           "    <h2>Les intérêts politiques de Google Maps :</h2>\n"
           "    <img src=\"../images/finland.png\" alt=\"Carte truquée\">\n"
           "    <ul>\n"
           "        <li><a href=\"theories/finlande.html\">La Finlande n'existe pas</a></li>\n"
           "    </ul>\n"
           "\n"
           "    <h2>La mafia Haribo :</h2>\n"
           "    <img src=\"../images/haribo.png\" alt=\"Dragibus suspects\">\n"
           "    <ul>\n"
           "        <li><a href=\"theories/haribo.html\">Le financement des réseaux criminels</a></li>\n"
           "    </ul>\n"
           "\n"
           "    <h2>Autres liens utiles :</h2>\n"
           "    <img src=\"../asset/404.png\" alt=\"Sources externes\">\n"
           "    <ul>\n"
           "        <li><a href=\"https://www.gnu.org/gnu/manifesto.html\">Manifeste GNU</a></li>\n"
           "        <li><a href=\"https://theflatearthsociety.org/home/\">The Flat Earth Society</a></li>\n"
           "        <li><a href=\"https://www.reddit.com/r/conspiracy/\">Subreddit /r/conspiracy</a></li>\n"
           "    </ul>\n"
           "</body>\n"
           "</html>\n";
}


void HttpResponse::build(HttpRequest &request) {

    if (request.getMethod() == "GET") {
        handleGet(request, *this);
    }
    else if (request.getMethod() == "POST") {
        handlePost(request, *this);
    }
    else if (request.getMethod() == "DELETE") {
        handleDelete(request, *this);
    }
}
