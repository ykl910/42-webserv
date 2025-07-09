#include "../include/Socket.hpp"

Socket::Socket() {
    bzero(&this->_hints, sizeof(this->_hints));
    this->_hints.ai_family = AF_INET;
    this->_hints.ai_socktype = SOCK_STREAM;
    this->_hints.ai_flags = AI_PASSIVE;
    struct addrinfo *servInfosLst = NULL;

    int status;
    status = getaddrinfo(NULL, "8080", &this->_hints, &servInfosLst);
    if(status != 0)
        this->printGaiErrorAndThrow("getaddrinfo", status);

    struct addrinfo *chosenAddr = servInfosLst;

    while (chosenAddr != NULL)
    {
        this->_serverFd = socket(chosenAddr->ai_family, chosenAddr->ai_socktype, chosenAddr->ai_protocol);
        if(this->_serverFd == -1){
            this->printError();
            chosenAddr = chosenAddr->ai_next;
            continue;
        }
        break;
    }
    if (!chosenAddr)
        this->printErrorAndThrow("socket");

    int opt = 1;
    if (setsockopt(this->_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        this->printErrorAndThrow("setsockopt");

    if (bind(this->_serverFd, chosenAddr->ai_addr, chosenAddr->ai_addrlen) == -1)
        this->printErrorAndThrow("bind");
    this->_isBound = true;

    freeaddrinfo(servInfosLst);

    if (listen(this->_serverFd, SOMAXCONN) == -1)
        this->printErrorAndThrow("listen");
    this->_isListening = true;
}

Socket::~Socket() {

}
