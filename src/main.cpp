#include "../include/WebServ.hpp"

/* Authorized functions
char *strerror(int errnum);
const char *gai_strerror(int errcode);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

int close(int fd);
int closedir(DIR *dirp);
int chdir(const char *path);
DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);

pid_t fork(void);
int dup(int oldfd);
int dup2(int oldfd, int newfd);
int pipe(int pipefd[2]);
int execve(const char *pathname, char *const argv[], char *const envp[]);

int listen(int sockfd, int backlog);
int socket(int domain, int type, int protocol);
int socketpair(int domain, int type, int protocol, int sv[2]);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);
int setsockopt(int sockfd, int level, int optname,
                    const void *optval, socklen_t optlen);
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
struct protoent *getprotobyname(const char *name);
pid_t waitpid(pid_t pid, int *wstatus, int options);
int kill(pid_t pid, int sig);
sighandler_t signal(int signum, sighandler_t handler);
int access(const char *pathname, int mode);
int stat(const char *pathname, struct stat *statbuf);
*/

void    createSocket(void) {

}

bool    got_config_file(int argc) {
    if (argc == 2)
        return true;
    return false;
}

void    print_usage(void) {
    std::cout
    << BOLD WHITE << "./webserv [configuration file]\n" << DEFAULT;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        if (got_config_file(argc)) {
            WebServ webserv();
        } else {
            WebServ webserv;
        }
        createSocket();
        return EXIT_SUCCESS;
    }
    print_usage();
    return EXIT_FAILURE;
}
