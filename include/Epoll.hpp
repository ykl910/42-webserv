#pragma once

#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/epoll.h>
#include <vector>
#include <map>

#define MAXEVENTS 4096
#define BUFFERSIZE 512

template <class Multiplexer>
class WebServ;

class Epoll : public Socket {
public:
    typedef struct epoll_event epoll_ev;
    typedef std::vector<epoll_ev> vector;
    void createEpollInstance();
    int acceptClient();
    void addServerToEpool();
    void addClientToEpool(int const &clientFd);
    void run(WebServ<Epoll>& server);
    void sendHttpResponse(int &clientFd, HttpRequest &request);
    bool receivedCompleteRequest(std::string &rawData) const;
    const int& getEpollFd(void) const;
    void eventManager(epoll_ev &event);
    void HttpRequestAndResponse(int &clientfd);
    Epoll();
    ~Epoll();

private:
    int _epollFd;
    std::map<int, std::string> _buffers;

};
