#include "../../include/Server.hpp"

Socket& Server::getSocket(void)
{
    return _socket;
}

int Server::getSocketFd(void) const
{
    return _socket.getSocketFd();
}

t_serv_attr& Server::getServerAttribute(void)
{
    return _attribute;
}

int Server::getClientMaxBodySize(const std::string& input)
{
    size_t result;

    if (input.find_first_of("kKmMgG") != std::string::npos) {
        char unit(input.substr(input.length() - 1, 1)[0]);

        result = std::atol(input.substr(0, input.length() - 1).c_str());
        if (unit == 'k' || unit == 'K')
            result *= 1024;
        else if (unit == 'm' || unit == 'M')
            result *= 1024 * 1024;
        else if (unit == 'g' || unit == 'G')
            result *= 1024 * 1024 * 1024;

    } else
        result = std::atol(input.substr(0, input.length()).c_str());

    return result;
}

void Server::storeErrorPage(server& config, size_t locationNbr) {
    std::string root(_attribute.rootLocation);

    _attribute.error_page.err_400 =
        root + "/" + config[ERROR + locationNbr][E_400][0];
    _attribute.error_page.err_403 =
        root + "/" + config[ERROR + locationNbr][E_403][0];
    _attribute.error_page.err_404 =
        root + "/" + config[ERROR + locationNbr][E_404][0];
    _attribute.error_page.err_405 =
        root + "/" + config[ERROR + locationNbr][E_405][0];
    _attribute.error_page.err_500 =
        root + "/" + config[ERROR + locationNbr][E_500][0];
}

void Server::getRedirectionValue(server& config, size_t locationNbr,
                                 std::vector<std::string>& redir, int i)
{
    std::string value;
    std::stringstream redirectionValue(config[REDIRECTION + locationNbr][i][0]);

    size_t j = 0;
    while (redirectionValue >> value) {
        if (j > 2)
            throw std::runtime_error("Error: redirection got too much arguments");
        redir.push_back(value);
        ++j;
    }
}

void Server::storeRedirection(server& config, size_t locationNbr)
{
    getRedirectionValue(config, locationNbr,
                                _attribute.redirection.redir_301, 0);
    getRedirectionValue(config, locationNbr,
                                _attribute.redirection.redir_302, 1);
}

bool Server::methodAlreadyDefined(uint8_t mask, size_t method)
{
    return mask & 1 << method;
}

void Server::storeLocation(server& config, size_t locationNbr)
{
    for (size_t i = 0; i < locationNbr; ++i) {
        t_location newLocation;

        newLocation.path = config[LOCATION + i][PATH][0];
        newLocation.root = config[LOCATION + i][ROOT][0];
        newLocation.index = config[LOCATION + i][INDEX][0];

        if (config[LOCATION + i][AUTOINDEX][0] == "on")
            newLocation.autoindex = true;
        else if (config[LOCATION + i][AUTOINDEX][0] == "off")
            newLocation.autoindex = false;

        std::string method;
        std::stringstream methodDirective(config[LOCATION + i][METHOD][0]);

        newLocation.method = 0;
        while (methodDirective >> method) {
            if (method == "GET"
                && !methodAlreadyDefined(newLocation.method, GET))
                newLocation.method |= 1 << GET;
            else if (method == "POST"
                && !methodAlreadyDefined(newLocation.method, POST))
                newLocation.method |= 1 << POST;
            else if (method == "DELETE"
                && !methodAlreadyDefined(newLocation.method, DELETE))
                newLocation.method |= 1 << DELETE;
        }
        _attribute.location.push_back(newLocation);
        _attribute.locMap.insert(
            std::pair<std::string, t_location>(newLocation.path, newLocation));
    }
}

void Server::storeCgi(server& config, size_t locationNbr, size_t cgiNbr)
{
    static size_t               total;
    std::string                 extension;
    std::string                 path;
    std::vector<std::string>    cgiPath;

    for (size_t i = 0; i < cgiNbr; ++i) {
        std::stringstream cgiDirectiveSplitter(config[CGI + locationNbr + total][i][0]);

        cgiDirectiveSplitter >> extension;
        while (cgiDirectiveSplitter >> path) {
            cgiPath.push_back(path);
        }
        if (!_attribute.cgi.empty()) {
            cgiMapIterator it = _attribute.cgi.find(extension);

            if (it != _attribute.cgi.end())
                throw std::runtime_error("Error: cgi extension already used");
        }
        _attribute.cgi.insert(std::pair<std::string,
                              std::vector<std::string> >(extension, cgiPath));
    }
    total += cgiNbr;
}

void    Server::initSocket()
{
    _socket.createSocket(_attribute.host.c_str(), _attribute.port.c_str(), 0);
}

Server::Server(server& config, size_t locationNbr, size_t cgiNbr) : _socket()
{
    _attribute.port = config[SERVER][LISTEN][0];
    _attribute.host = config[SERVER][HOST][0];
    _attribute.server_name = config[SERVER][SERVER_NAME][0];
    _attribute.client_max_body_size =
    getClientMaxBodySize(config[SERVER][CLIENT_MAX_BODY_SIZE][0]);
    _attribute.rootLocation = config[SERVER][ROOT_LOCATION][0];
    storeLocation(config, locationNbr);
    storeErrorPage(config, locationNbr);
    storeRedirection(config, locationNbr);
    storeCgi(config, locationNbr, cgiNbr);
}

Server::~Server() {}
