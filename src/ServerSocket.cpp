#include "../include/ServerSocket.hpp"
#include "../include/httpRequest.hpp"

void    ServerSocket::printError() const {
    std::cerr << "Error: " << strerror(errno) << std::endl;
}

void    ServerSocket::printServerStatus(const char* multiplexer) const {
    std::cout << BOLD WHITE << "Server status: "
    << BOLD ITALIC GREEN << "running\n" << DEFAULT
    << BOLD WHITE << "Port: "
    << BOLD ITALIC BLUE  << "8080\n" << DEFAULT
    << BOLD WHITE << "Multiplexer: "
    << BOLD ITALIC BLUE  << multiplexer << DEFAULT
    << std::endl;
}

int ServerSocket::getServerFd() const {
    return this->_serverFd;
}

void    ServerSocket::bindAndListen() {

    //* Bind the socket to the IP:port from serverAddress
    errno = 0;
    if (bind(this->_serverFd, (struct sockaddr *)&this->_serverAddress, this->_addrlen) < 0) {
        printError();
        close(this->_serverFd);
        exit(EXIT_FAILURE);
    }
    this->_isBound = true;

    //* Set the socket in listening mode (accepting connexions)
    //* SOMAXCONN = macro for max connexion (4096)
    errno = 0;
    if (listen(this->_serverFd, SOMAXCONN) < 0) {
        printError();
        close(this->_serverFd);
        exit(EXIT_FAILURE);
    }
    this->_isListening = true;
}

std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello from webserv!</h1></body></html>";

void    ServerSocket::acceptClientPoll() {

    // struct pollfd = poll(pollfds, )
    // int poll(struct pollfd *fds, nfds_t nfds, int timeout);
    while (true) {

    }
}

void    ServerSocket::acceptClientEpoll() {
    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
        throw std::runtime_error("Error: epoll create");

    while (true) {

    }

    // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
}

void    ServerSocket::acceptClientSelect() {
    fd_set readFds;
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    int maxFd = this->_serverFd;

    while (true) {

        //* FD_ZERO = empty readFds set
        //* FD_SET = add server socket to detect new connexions
        FD_ZERO(&readFds);
        FD_SET(this->_serverFd, &readFds);

        //* Add everyt client sockets to readFds and add maxFd if necessary
        for (fdsIterator it = this->_clientFds.begin();
            it != this->_clientFds.end(); ++it) {
            FD_SET(*it, &readFds);
            if (*it > maxFd)
                maxFd = *it;
        }

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        //* wait for event in a socket
        errno = 0;
        int activity = select(maxFd + 1, &readFds, NULL, NULL, &tv);
        if (activity < 0) {
            printError();
            continue;
        }

        //* new connexion -> accept connexion and add client to the list
        if (FD_ISSET(this->_serverFd, &readFds)) {
            errno = 0;
            int newClient = accept(this->_serverFd, NULL, NULL);
            if (newClient < 0) {
                printError();
                continue;
            } else {
                this->_clientFds.push_back(newClient);
                std::cout << "New client connected: FD " << newClient << std::endl;
            }
        }

        //* loop on every actives clients and seek for data to read
        for (fdsIterator it = this->_clientFds.begin(); it != this->_clientFds.end();) {
            char buf[4096];
            int bytes = 0;

            if (FD_ISSET(*it, &readFds)) {
                bytes = recv(*it, buf, sizeof(buf), 0);
                if (bytes <= 0) {
                    close(*it);
                    std::cout << "Client disconnected: FD " << *it << std::endl;
                    it = this->_clientFds.erase(it);
                    continue;
                }

                std::string request(buf, bytes);
                httpRequest httpReq(request);
                std::cout << "Received request:\n" << request << std::endl;
                send(*it, response.c_str(), response.size(), 0);
                close(*it);
                it = this->_clientFds.erase(it);
            }
            else
                ++it;
        }
    }
}

void    ServerSocket::initServer(const std::string& config_file) {
    (void)config_file;
}

ServerSocket::ServerSocket(const char* config_file) : _config_file(config_file) {

    //* Create a new socket
    //* AF_INET = IPv4
    //* SOCK_STREAM = TCP protocol
    //* 0 = default protocol
    errno = 0;
    if ((this->_serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printError();
        exit(EXIT_FAILURE);
    }

    //* Set socket options to reuse the same port each time
    //* SOL_SOCKET = general options
    //* SO_REUSEADDR = allow to reuse (IP:port) already used
    //* opt = activate/desactivate
    errno = 0;
    int opt = 1;
    if (setsockopt(this->_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printError();
        exit(EXIT_FAILURE);
    }

    //* AF_INET = IPv4
    //* INADDR_ANY = accept all network interfaces connexions
    //* PORT = 8080 (-> macro)

    bzero(&this->_serverAddress, sizeof(struct sockaddr_in));
    this->_serverAddress.sin_family = AF_INET;
    this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
    this->_serverAddress.sin_port = htons(PORT);

    this->_isBound = false;
    this->_isListening = false;

    this->_addrlen = sizeof(this->_serverAddress);
}

ServerSocket::~ServerSocket() {
    if (this->_serverFd)
        close(this->_serverFd);
    for (fdsIterator it = this->_clientFds.begin(); it != this->_clientFds.end(); ++it)
        close(*it);
}
