#include "../include/Socket.hpp"

int Socket::getSocketFd() const {
    return this->_socketFd;
}

void Socket::setSocketOpt() {

    int opt = 1;
    if (setsockopt(this->_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        printErrorAndThrow("setsockopt");

    int flags = fcntl(this->_socketFd, F_GETFL, 0);
    if (flags == -1)
        printErrorAndThrow("fcntl()");

    flags |= O_NONBLOCK;

    int s = fcntl(this->_socketFd, F_SETFL, flags);
    if (s == -1)
        printErrorAndThrow("fcntl()");
}

void Socket::createAndBind() {
    this->_hints.ai_family = AF_INET;
    this->_hints.ai_socktype = SOCK_STREAM;
    this->_hints.ai_flags = AI_PASSIVE;
    struct addrinfo *servInfosLst = NULL;

    int status;
    status = getaddrinfo(NULL, "8080", &this->_hints, &servInfosLst);
    if (status != 0)
        printGaiErrorAndThrow("getaddrinfo", status);

    struct addrinfo *chosenAddr = servInfosLst;

    while (chosenAddr != NULL) {
        this->_socketFd = socket(chosenAddr->ai_family, chosenAddr->ai_socktype, chosenAddr->ai_protocol);
        if(this->_socketFd == -1) {
            printError();
            chosenAddr = chosenAddr->ai_next;
            continue;
        }
        break;
    }
    if (!chosenAddr)
        printErrorAndThrow("socket");

    this->setSocketOpt();

    if (bind(this->_socketFd, chosenAddr->ai_addr, chosenAddr->ai_addrlen) == -1)
        printErrorAndThrow("bind");

    freeaddrinfo(servInfosLst);
}

void Socket::setOnListening(){
    if (listen(this->_socketFd, SOMAXCONN) == -1)
        printErrorAndThrow("listen");
}

Socket::Socket() {
    this->createAndBind();
    this->setOnListening();
}

Socket::~Socket() {
    if (this->_socketFd)
        close(this->_socketFd);
}
