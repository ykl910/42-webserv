#include "../include/Poll.hpp"
#include "../include/HttpManager.hpp"
#include "../include/WebServ.hpp"

/*
    struct pollfd {
        int   fd;         file descriptor
        short events;     requested events
        short revents;    returned events
    };
*/

void    Poll::addClientToPoll(int clientFd)
{
    struct pollfd newClient;

    newClient.fd = clientFd;
    newClient.events = POLLIN;
    newClient.revents = 0;
    _pollFd.push_back(newClient);
    std::cout << BOLD WHITE << "Poll: new client accepted with fd "
    << BOLD BLUE << newClient.fd << DEFAULT << "\n";
}

void    Poll::initPoll(void)
{
    struct pollfd serverPoll;

    serverPoll.fd = getSocketFd();
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;
    _pollFd.push_back(serverPoll);
}

void    Poll::run(WebServ<Poll>& server)
{
    initPoll();
    server.printServerStatus("poll", getConfigFilePath());
    while (true) {
        _activity = poll(&_pollFd[0], _pollFd.size(), 10); // 10 ms timeout
        if (_activity == -1)
            printError();

        // if socket got a new client
        if (_pollFd[0].revents & POLLERR)
            std::cout << "Poll: error catched from socket fd.\n";
        else if (_pollFd[0].revents & POLLIN) {
            int clientFd = acceptClient();
            if (clientFd)
                addClientToPoll(clientFd);
        }

        for (pollIterator it = _pollFd.begin() + 1; // skip socket fd
                          it != _pollFd.end();) {
            if (it->revents & POLLERR)
                std::cout << "Poll: error catched for client "
                << it->fd << "\n";
            else if (it->revents & POLLHUP)
                std::cout << "Poll: connexion closed for client "
                << it->fd << "\n";
            else if (it->revents & POLLIN) {
                HttpManager(it->fd);
                close(it->fd);
                it = _pollFd.erase(it);
            } else
                ++it;
        }
    }
}

Poll::Poll(const char* configFilePath) : Socket(configFilePath) {}

Poll::~Poll()
{
    for (pollIterator it = _pollFd.begin();
                      it != _pollFd.end(); it++) {
        if (it->fd)
            close(it->fd);
    }
}
