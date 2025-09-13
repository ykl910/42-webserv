#include "../../include/HttpManager.hpp"

#define CONTENT_LENGTH_SIZE 16

std::map<int, std::string>   HttpManager::_buffers;
std::map<int, HttpRequest>   HttpManager::_request;
std::map<int, bool>          HttpManager::_gotResponse;
std::map<int, bool>          HttpManager::_gotFullRequest;
std::map<int, int>           HttpManager::_pendingResponse;
std::map<int, HttpResponse>  HttpManager::_responses;

bool HttpManager::hasCompletedResponse(int clientFd) {
    return _pendingResponse.find(clientFd) == _pendingResponse.end();
}

void    HttpManager::writeUserInfo(HttpRequest &request, HttpResponse &response)
{
    std::map<std::string, std::string>::const_iterator mapit;
    std::string sessionId;
    for (mapit = request.getHeaders().begin();
        mapit != request.getHeaders().end(); ++mapit) {
        if (mapit->first == "Cookie") {
            size_t sesStartPos = mapit->second.find("sessionid=");
            if (sesStartPos != std::string::npos) {
                sessionId = mapit->second.substr(sesStartPos + 8, 18);
                break;
            }
        }
    }

    if (!sessionId.empty()) {

        std::string server_path = response.getRoot() + "/sessions";
        struct stat st;
        memset(&st, 0, sizeof(st));
        if (stat(server_path.c_str(), &st) == -1) {
            if (mkdir(server_path.c_str(), 0755) == -1) {
                printError();
                return;
            }
        }
        std::string fullpath = server_path + "/sessionLog_" + sessionId + ".txt";
        int fd = open(fullpath.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (fd == -1) {
            printError();
            return;
        }

        for (mapit = request.getHeaders().begin();
            mapit != request.getHeaders().end(); ++mapit) {
            if (mapit->first == "Cookie") {
                std::string req = response.getRequestAttr().method + " " + response.getRequestAttr().path + "\n";
                std::string res = response.getStatusLine() + "\n";
                std::string tot = "request: " + req + "response: " + res;
                write(fd, tot.c_str(), tot.length());
                break;
            }
        }
        close(fd);
    }
}

void    HttpManager::sendResponse(int clientFd, HttpRequest& request,
                                  t_serv_attr& servAttr, int &clientState) {
    std::string response;

    if (!_gotResponse[clientFd])
    {
        std::cout
        << BLUE << "client [" << clientFd << "]: No pending response"
        << DEFAULT << std::endl;

        HttpResponse Response(request, servAttr);
        writeUserInfo(request, Response);
        response = Response.getResponse();
        _responses[clientFd] = Response;
        _gotResponse[clientFd] = true;
    }
    else
    {
        std::cout
        << BLUE << "client [" << clientFd << "]: pending response"
        << DEFAULT << std::endl;

        response = _responses[clientFd].getResponse();
    }

    size_t totalBytesSent = _pendingResponse[clientFd];
    size_t responseLen = response.length();

    std::cout
    << BLUE << "client [" << clientFd << "]: " << "responseLen = "
    << responseLen
    << DEFAULT << std::endl;

    std::cout
    << BLUE << "client [" << clientFd << "]: "
    << "totalBytesSent before loop = " << totalBytesSent
    << DEFAULT << std::endl;

    while (totalBytesSent < responseLen)
    {
        ssize_t bytesSent = send(clientFd, response.c_str() + totalBytesSent, responseLen - totalBytesSent, 0);
        std::cout
        << BLUE << "client [" << clientFd << "]: "
        << "bytesSent inside loop = " << bytesSent
        << DEFAULT << std::endl;

        if (bytesSent <= 0)
            break ;
        totalBytesSent += bytesSent;
    }

    std::cout
    << BLUE << "client [" << clientFd << "]: "
    << "totalBytesSent after loop = " << totalBytesSent
    << DEFAULT << std::endl;

    if (totalBytesSent != responseLen)
    {
        clientState = RESPONSE_TRUNCATE;
        _pendingResponse[clientFd] = totalBytesSent;
    }
    else
    {
        std::cout
        << BLUE << "client [" << clientFd << "]: Response Sent"
        << DEFAULT << std::endl;

        std::cout
        << BLUE << "client [" << clientFd << "]: Response: \n" << "------------------------------\n "
        << ITALIC GREEN << response.substr(0,1000) << BLUE << "\n------------------------------\n"
        << DEFAULT << std::endl;

        _request.erase(clientFd);
        _pendingResponse.erase(clientFd);
        _gotResponse.erase(clientFd);
        _responses.erase(clientFd);
        clientState = SENT;
    }
}

bool    HttpManager::receivedCompleteRequest(std::string &rawData, t_serv_attr &serverAttr) const {

    size_t headerEnd = rawData.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;

    size_t bodyStart = headerEnd + 4;

    size_t contentLengthPos = rawData.find("Content-Length: ");
    if (contentLengthPos == std::string::npos)
        return true;

    size_t valueStart = contentLengthPos + CONTENT_LENGTH_SIZE;
    size_t valueEnd = rawData.find("\r\n", valueStart);
    std::string valueStr = rawData.substr(valueStart, valueEnd - valueStart);
    int contentLength = std::atoi(valueStr.c_str());

    size_t bodyLengh = rawData.size() - bodyStart;
    (void)serverAttr;
    return bodyLengh >= static_cast<size_t>(contentLength);

}

inline bool    HttpManager::checkPersistance(std::string &rawData)
{
    return (rawData.find("Connection: keep-alive") != std::string::npos);
}

void    HttpManager::getRequest(int clientFd,t_serv_attr &serverAttr, int &clientState, bool &persistance) {

    char buffer[4096];

    int bytes = recv(clientFd, buffer, sizeof(buffer), 0);
    {
        std::cout
        << CYAN << "client [" << clientFd << "]: bytes received = "
        << bytes << DEFAULT << std::endl;

        if (bytes > 0)
            _buffers[clientFd].append(buffer, bytes);
        if (bytes == 0 || bytes == -1)
        {
            clientState = SENT;
            persistance = false;
            return ;
        }
    }

    if (receivedCompleteRequest(_buffers[clientFd], serverAttr)) {

        std::cout
        << CYAN << "client [" << clientFd << "]: Request received"
        << DEFAULT << '\n'
        << CYAN << "client [" << clientFd << "]: Request:\n"
        << "------------------------------\n"
        << ITALIC GREEN << _buffers[clientFd].substr(0, 500)
        << CYAN << "\n------------------------------\n" << DEFAULT << '\n';

        persistance = checkPersistance(_buffers[clientFd]);
        clientState = RECEIVED;
        HttpRequest request(_buffers[clientFd]);
        _request[clientFd] = request;
        _buffers.erase(clientFd);
        _pendingResponse[clientFd] = 0;
    }
}

HttpManager::HttpManager(int clientFd, t_serv_attr &serverAttr, int &clientState, bool &persistance)
{
    if (clientState == PENDING)
        getRequest(clientFd, serverAttr, clientState, persistance);

    if (clientState == RECEIVED || clientState == RESPONSE_TRUNCATE)
        sendResponse(clientFd, _request[clientFd], serverAttr, clientState);
}

HttpManager::~HttpManager() {}
