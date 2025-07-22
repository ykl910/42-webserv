#include "../include/Socket.hpp"

int Socket::getSocketFd() const {
    return _socketFd;
}

void Socket::setSocketOpt() {

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

void Socket::createAndBind() {
    bzero(&_hints, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;
    struct addrinfo *servInfosLst = NULL;

    int status;
    status = getaddrinfo(NULL, "8080", &_hints, &servInfosLst);
    if (status != 0)
        printGaiErrorAndThrow("getaddrinfo", status);

    struct addrinfo *chosenAddr = servInfosLst;

    while (chosenAddr != NULL) {
        _socketFd = socket(chosenAddr->ai_family, chosenAddr->ai_socktype, chosenAddr->ai_protocol);
        if(_socketFd == -1) {
            printError();
            chosenAddr = chosenAddr->ai_next;
            continue;
        }
        break;
    }
    if (!chosenAddr)
        printErrorAndThrow("socket");

    setSocketOpt();

    if (bind(_socketFd, chosenAddr->ai_addr, chosenAddr->ai_addrlen) == -1)
        printErrorAndThrow("bind");

    freeaddrinfo(servInfosLst);
}

void Socket::setOnListening(){
    if (listen(_socketFd, SOMAXCONN) == -1)
        printErrorAndThrow("listen");
}

Socket::Socket(const char *configFilePath) : Config(configFilePath) {
    createAndBind();
    setOnListening();
}

Socket::~Socket() {
    if (_socketFd)
        close(_socketFd);
}
