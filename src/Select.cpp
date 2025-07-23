#include "../include/Select.hpp"
#include "../include/WebServ.hpp"

bool Select::acceptClient(int serverFd) {
    if (FD_ISSET(serverFd, &_readFds)) {
    /*
        select() modifies the contents of the sets according to the rules
        described below. After  calling  select(),  the FD_ISSET() macro
        can be used to test if a file descriptor is still present in a set.
        FD_ISSET() returns nonzero  if  the file descriptor fd is present
        in set, and zero if it is not.
    */
        errno = 0;
        int newClient = accept(serverFd, NULL, NULL);
        if (newClient < 0) {
            printError();
            return true;
        } else {
            int flags = fcntl(newClient, F_GETFL, 0);
            if (flags == -1 || fcntl(newClient, F_SETFL, flags | O_NONBLOCK) == -1) {
                printError();
                close(newClient);
            } else {
                _selectFd.push_back(newClient);
                std::cout << "New client connected: FD " << newClient << std::endl;
            }
        }
    }
    return false;
}

HttpRequest    Select::readCompleteRequest(std::string *requestData, int fd, bool &success)
{
    char buf[4096];
    size_t totalBytes = 0;
    size_t contentLength = 0;
    bool headersComplete = false;
    size_t headerEnd = 0;
    while (!headersComplete)
    {
        ssize_t bytes = recv(fd, buf, sizeof(buf), 0);
        if (bytes < 0) {
            continue;
        }
        else if (bytes == 0) {
            success = false;
            return HttpRequest();
        }
        requestData->append(buf, bytes);
        totalBytes += bytes;
        headerEnd = requestData->find("\r\n\r\n");
        if (headerEnd != std::string::npos)
        {
            headersComplete = true;
            size_t clPos = requestData->find("Content-Length:");
            if (clPos != std::string::npos)
            {
                size_t valueStart = requestData->find_first_not_of(" ", clPos + 15);
                size_t valueEnd = requestData->find("\r\n", valueStart);
                std::string len = requestData->substr(valueStart, valueEnd - valueStart);
                contentLength = atoi(len.c_str());
            }
        }
    }
    while (requestData->size() < headerEnd + 4 + contentLength) {
        ssize_t bytes = recv(fd, buf, sizeof(buf), 0);
        if (bytes < 0) {
            continue;
        }
        else if (bytes == 0) {
            success = false;
            return HttpRequest();
        }
        requestData->append(buf, bytes);
    }
    HttpRequest httpReq(*requestData);
    success = true;
    return(httpReq);
}

void    Select::manageRequest(void) {
    for (selectIterator it = _selectFd.begin();
                        it != _selectFd.end();) {
            HttpRequest httpReq;
            std::string requestData;
            if (FD_ISSET(*it, &_readFds)) {
                bool success = false;
                httpReq = readCompleteRequest(&requestData, *it, success);
                if (!success) {
                    close(*it);
                    it = _selectFd.erase(it);
                    continue;
                }
            std::cout << BOLD ITALIC GREEN <<  "Request:\n" << DEFAULT;
            std::cout << MAGENTA << requestData << DEFAULT << std::endl;
            HttpResponse httpRes(httpReq);
            std::string response = httpRes.getResponse();
            std::cout << BOLD ITALIC GREEN <<  "Response:\n" << DEFAULT;
            std::cout << YELLOW << httpRes.getStatusLine() << DEFAULT << std::endl;
            writeUserInfo(httpReq, httpRes);
            ssize_t totalSent = 0;
            const char* data = response.c_str();
            ssize_t totalSize = response.size();
            int retry = 1000;
            while (totalSent < totalSize && retry-- > 0) {
                ssize_t sent = send(*it, data + totalSent, totalSize - totalSent, 0);
                if (sent <= 0) {
                    continue;
                }
                totalSent += sent;
            }
            close(*it);
            it = _selectFd.erase(it);
        }
        else
            ++it;
        }
    }


void    Select::run(WebServ<Select>& server) {
    server.printServerStatus("select", getConfigFilePath());

    _tv.tv_sec = 10;
    _tv.tv_usec = 0;
    int maxFd = getSocketFd();
    int serverFd = getSocketFd();
    int activity = 0;

    while (true) {
        FD_ZERO(&_readFds);
        /*
            This  macro  clears  (removes all file descriptors from) set.
            It should be employed as the first step in initializing a file
            descriptor set.
        */
        FD_SET(serverFd, &_readFds);
        /*
            This macro adds the file descriptor fd to set. Adding a file
            descriptor that is already present in the set is a no-op, and does
            not produce an error.
        */
        for (selectIterator it = _selectFd.begin();
                            it != _selectFd.end(); ++it) {
            FD_SET(*it, &_readFds);
            if (*it > maxFd)
                maxFd = *it;
        }
        //* wait for event in a socket
        errno = 0;
        activity = select(maxFd + 1, &_readFds, NULL, NULL, &_tv);
        if (activity < 0) {
            printError();
            continue;
        } else if (activity == 0) {
            continue;
        }
        //* new connexion -> accept connexion and add client to the list
        acceptClient(serverFd);
        //* read request and send response
        manageRequest();
    }
}

Select::Select(const char *configFilePath) : Socket(configFilePath) {}

Select::~Select() {
    for (selectIterator it = _selectFd.begin();
         it != _selectFd.end(); ++it)
        close(*it);
}
