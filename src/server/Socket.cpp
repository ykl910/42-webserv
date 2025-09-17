#include "../../include/Socket.hpp"

std::vector<std::string> Socket::_portsUsed;

int Socket::getSocketFd() const
{
    return _socketFd;
}

int Socket::acceptClient(void)
{
    errno = 0;
    int clientFd = accept(_socketFd, NULL, NULL);

    if (clientFd == -1)
        printErrorAndThrow("accept");

    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
        printErrorAndThrow("fcntl");
    return clientFd;
}

void Socket::setSocketOpt()
{
    int opt = 1;
    if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        printErrorAndThrow("setsockopt");

    int flags = fcntl(_socketFd, F_GETFL, 0);

    if (flags == -1)
        printErrorAndThrow("fcntl()");

    flags |= O_NONBLOCK;

    int s = fcntl(_socketFd, F_SETFL, flags);
    if (s == -1)
        printErrorAndThrow("fcntl()");
}

bool    Socket::portAlreadyUsed(const std::string& port)
{
    for (size_t i = 0; i < _portsUsed.size(); ++i) {
        if (_portsUsed[i] == port)
            return true;
    }
    return false;
}

void Socket::createSocket(const char* host, const char* port, bool state)
{
    if (state == ALREADY_USED)
        return;
    bzero(&_hints, sizeof(_hints));
    _hints.ai_flags = AI_PASSIVE;
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_protocol = 0;
    _hints.ai_addrlen = 0;
    _hints.ai_addr = NULL;
    _hints.ai_canonname = NULL;
    _hints.ai_next = NULL;

    struct addrinfo *servInfosLst = NULL;
    int status;

    status = getaddrinfo(host, port, &_hints, &servInfosLst);
    if (status != 0)
        printGaiErrorAndThrow("getaddrinfo", status);

    struct addrinfo *chosenAddr = servInfosLst;

    while (chosenAddr != NULL) {
        _socketFd = socket(chosenAddr->ai_family, chosenAddr->ai_socktype,
                           chosenAddr->ai_protocol);
        if (_socketFd == -1) {
            printError();
            chosenAddr = chosenAddr->ai_next;
            continue;
        } else
            break;
    }
    if (!chosenAddr)
        printErrorAndThrow("socket");

    setSocketOpt();

    if (bind(_socketFd, chosenAddr->ai_addr, chosenAddr->ai_addrlen) == -1)
        printErrorAndThrow("bind");

    freeaddrinfo(servInfosLst);

    if (listen(_socketFd, SOMAXCONN) == -1)
        printErrorAndThrow("listen");
    _portsUsed.push_back(port);
}

Socket::Socket() {}

Socket::~Socket() {}
