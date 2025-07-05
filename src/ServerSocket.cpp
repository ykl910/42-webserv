#include "../include/ServerSocket.hpp"

void ServerSocket::printError()
{
    std::cerr << "Error: " << strerror(errno) << std::endl;
}

int ServerSocket::getServerFd() const {
    return serverFd;
}

void ServerSocket::bindAndListen() {

    //* Bind the socket to the IP:port from serverAddress

    errno = 0;
    if (bind(serverFd, (struct sockaddr *)&serverAddress, addrlen) < 0)
    {
        printError();
        close(serverFd);
        exit(EXIT_FAILURE);
    }
    isBound = true;

    //* Set the socket in listening mode (accepting connexions)
    //* SOMAXCONN = macro for max connexion (4096)

    errno = 0;
    if (listen(serverFd, SOMAXCONN) < 0) {
        printError();
        close(serverFd);
        exit(EXIT_FAILURE);
    }
    isListening = true;
}

std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello from webserv!</h1></body></html>";

void ServerSocket::acceptClient() {
    fd_set readFds;
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    int maxFd = serverFd;

    while (true)
    {
        //* FD_ZERO = empty readFds set
        //* FD_SET = add server socket to detect new connexions

        FD_ZERO(&readFds);
        FD_SET(serverFd, &readFds);

        //* Add everyt client sockets to readFds and add maxFd if necessary

        for (fdsIterator it = clientFds.begin(); it != clientFds.end(); ++it)
        {
            FD_SET(*it, &readFds);
            if (*it > maxFd)
                maxFd = *it;
        }

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        //* wait for event in a socket

        errno = 0;
        int activity = select(maxFd + 1, &readFds, NULL, NULL, &tv);
        if (activity < 0)
        {
            printError();
            continue;
        }

        //* new connexion -> accept connexion and add client to the list

        if (FD_ISSET(serverFd, &readFds))
        {
            errno = 0;
            int newClient = accept(serverFd, NULL, NULL);
            if (newClient < 0) {
                printError();
                continue;
            }
            else
            {
                clientFds.push_back(newClient);
                std::cout << "New client connected: FD " << newClient << std::endl;
            }
        }

        //* loop on every actives clients and seek for data to read

        for (fdsIterator it = clientFds.begin(); it != clientFds.end();)
        {
            char buf[4096];
            int bytes = 0;

            if (FD_ISSET(*it, &readFds))
            {
                bytes = recv(*it, buf, sizeof(buf), 0);
                if (bytes <= 0)
                {
                    close(*it);
                    std::cout << "Client disconnected: FD " << *it << std::endl;
                    it = clientFds.erase(it);
                    continue;
                }

                std::string request(buf, bytes);
                std::cout << "Received request:\n" << request << std::endl;
                send(*it, response.c_str(), response.size(), 0);
                close(*it);
                it = clientFds.erase(it);
            }
            else
                ++it;
        }
    }
}

void    ServerSocket::initServer(const std::string& config_file) {
    (void)config_file;

}

ServerSocket::ServerSocket(const char* config_file) : config_file(config_file) {

    //* Create a new socket
    //* AF_INET = IPv4
    //* SOCK_STREAM = TCP protocol
    //* 0 = default protocol

    errno = 0;
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printError();
        exit(EXIT_FAILURE);
    }

    //* Set socket options to reuse the same port each time
    //* SOL_SOCKET = general options
    //* SO_REUSEADDR = allow to reuse (IP:port) already used
    //* opt = activate/desactivate

    errno = 0;
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printError();
        exit(EXIT_FAILURE);
    }

    //* AF_INET = IPv4
    //* INADDR_ANY = accept all network interfaces connexions
    //* PORT = 8080 (-> macro)

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    isBound = false;
    isListening = false;

    addrlen = sizeof(serverAddress);
}

ServerSocket::~ServerSocket() {
    if (serverFd)
        close(serverFd);
    for (fdsIterator it = clientFds.begin(); it != clientFds.end(); ++it)
        close(*it);
}
