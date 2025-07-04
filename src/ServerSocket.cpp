#include "../include/ServerSocket.hpp"
#include <vector>

ServerSocket::ServerSocket() {
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    isBound = 0;
    isListening = 0;
    addrlen = sizeof(serverAddress);
}

ServerSocket::~ServerSocket() {
    if (serverFd)
        close(serverFd);
    for (std::vector<int>::iterator it = clientFds.begin(); it != clientFds.end(); ++it)
        close(*it);
}

int ServerSocket::getServerFd() const {
    return serverFd;
}

void ServerSocket::bindAndListen() {
    if (bind(serverFd, (struct sockaddr *)&serverAddress, addrlen) < 0)
    {
        perror("bind");
        close(serverFd);
        exit(EXIT_FAILURE);
    }
    isBound = 1;
    if (listen(serverFd, SOMAXCONN) < 0) {
        perror("listen");
        close(serverFd);
        exit(EXIT_FAILURE);
    }
    isListening = 1;
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
        FD_ZERO(&readFds);
        FD_SET(serverFd, &readFds);
        for (std::vector<int>::iterator it = clientFds.begin(); it != clientFds.end(); ++it)
        {
            FD_SET(*it, &readFds);
            if (*it > maxFd)
                maxFd = *it;
        }
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        int activity = select(maxFd + 1, &readFds, NULL, NULL, &tv);
        if (activity < 0)
        {
            perror("select");
            continue;
        }
        if (FD_ISSET(serverFd, &readFds))
        {
            int newClient = accept(serverFd, NULL, NULL);
            if (newClient < 0) {
                perror("accept");
                continue;
            }
            else
            {
                clientFds.push_back(newClient);
                std::cout << "New client connected: FD " << newClient << std::endl;
            }
        }
        for (std::vector<int>::iterator it = clientFds.begin(); it != clientFds.end();)
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
