#include "../include/Socket.hpp"

/*
    struct addrinfo {
        int              ai_flags;
        int              ai_family;
        int              ai_socktype;
        int              ai_protocol;
        socklen_t        ai_addrlen;
        struct sockaddr *ai_addr;
        char            *ai_canonname;
        struct addrinfo *ai_next;
    };
*/

int Socket::acceptClient(void)
{
    errno = 0;
    int clientFd = accept(_socketFd, NULL, NULL);
    /*
       The  accept() system call is used with connection-based socket types
       (SOCK_STREAM, SOCK_SEQPACKET).  It extracts the first connection request
       on the queue of pending  connections  for the  listening  socket,
       sockfd,  creates a new connected socket, and returns a new file
       descriptor referring to that socket.  The newly created socket is not in
       the listening  state. The original socket sockfd is unaffected by this
       call.
    */
    if (clientFd == -1)
        printErrorAndThrow("accept");

    std::cout << "New pending connexion..." << std::endl;

    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
        printErrorAndThrow("fcntl");
    return clientFd;
}

int Socket::getSocketFd() const
{
    return _socketFd;
}

void Socket::setSocketOpt()
{
    int opt = 1;
    if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        printErrorAndThrow("setsockopt");

    int flags = fcntl(_socketFd, F_GETFL, 0);
    /*
        F_GETFL (void)
            Return  (as  the function result) the file access mode and the file
            status flags; arg is ignored.
    */
    if (flags == -1)
        printErrorAndThrow("fcntl()");

    flags |= O_NONBLOCK;

    int s = fcntl(_socketFd, F_SETFL, flags);
    if (s == -1)
        printErrorAndThrow("fcntl()");
}

void Socket::createAndBind()
{
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
    /*
       The getaddrinfo() function allocates and initializes a linked list of
       addrinfo  structures, one for each network address that matches node and
       service, subject to any restrictions imposed by hints, and returns a
       pointer to the start of the list in res.   The  items  in  the linked
       list are linked by the ai_next field.
    */
    status = getaddrinfo(NULL, "8080", &_hints, &servInfosLst);
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

    /*
       When  a socket is created with socket(2), it exists in a name space
       (address family) but has no address assigned to it.  bind() assigns the
       address specified by addr to the  socket  referred  to  by the file
       descriptor sockfd.  addrlen specifies the size, in bytes, of the address
       structure pointed to by addr.  Traditionally, this operation is  called
       “assigning  a name to a socket”.
    */
    if (bind(_socketFd, chosenAddr->ai_addr, chosenAddr->ai_addrlen) == -1)
        printErrorAndThrow("bind");

    freeaddrinfo(servInfosLst);
}

Socket::Socket(const char *configFilePath) : Config(configFilePath)
{
    createAndBind();
    /*
       listen()  marks  the  socket referred to by sockfd as a passive socket,
       that is, as a socket that will be used to accept incoming connection
       requests using accept(2).
    */
    if (listen(_socketFd, SOMAXCONN) == -1)
        printErrorAndThrow("listen");
}

Socket::~Socket()
{
    if (_socketFd)
        close(_socketFd);
}
