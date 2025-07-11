#include "../include/Socket.hpp"

int Socket::getServerFd() const {
    return this->_serverFd;
}

void Socket::setSocketOPt(){

    int opt = 1;
    if (setsockopt(this->_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        printErrorAndThrow("setsockopt");

    int flags = fcntl(this->_serverFd, F_GETFL, 0);
    if(flags == -1)
        printErrorAndThrow("fcntl()");

    flags |= O_NONBLOCK;

    int s = fcntl(this->_serverFd, F_SETFL, flags);
    if(s == -1)
        printErrorAndThrow("fcntl()");
}

void Socket::createAndBind(){

    bzero(&this->_hints, sizeof(this->_hints));
    this->_hints.ai_family = AF_INET;
    this->_hints.ai_socktype = SOCK_STREAM;
    this->_hints.ai_flags = AI_PASSIVE;
    addrinfo *servInfosLst = NULL;

    int status;
    status = getaddrinfo(NULL, "8080", &this->_hints, &servInfosLst);
    if (status != 0)
        printGaiErrorAndThrow("getaddrinfo", status);

    addrinfo *chosenAddr = servInfosLst;

    while (chosenAddr != NULL)
    {
        this->_serverFd = socket(chosenAddr->ai_family, chosenAddr->ai_socktype, chosenAddr->ai_protocol);
        if(this->_serverFd == -1){
            printError();
            chosenAddr = chosenAddr->ai_next;
            continue;
        }
        break;
    }
    if (!chosenAddr)
        printErrorAndThrow("socket");

    this->setSocketOPt();

    if (bind(this->_serverFd, chosenAddr->ai_addr, chosenAddr->ai_addrlen) == -1)
        printErrorAndThrow("bind");
    this->_isBound = true;

    freeaddrinfo(servInfosLst);
}

void Socket::setOnListening(){

    if (listen(this->_serverFd, SOMAXCONN) == -1)
        printErrorAndThrow("listen");
    this->_isListening = true;

}

Socket::Socket() {

    this->createAndBind();
    this->setOnListening();
}

Socket::~Socket() {
    if (this->_serverFd)
        close(this->_serverFd);
}
