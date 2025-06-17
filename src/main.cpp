/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: algadea <algadea@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 20:10:23 by algadea           #+#    #+#             */
/*   Updated: 2025/06/17 18:34:30 by algadea          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"

/* Authorized functions
int execve(const char *pathname, char *const argv[], char *const envp[]);
int pipe(int pipefd[2]);
char *strerror(int errnum);
const char *gai_strerror(int errcode);
int dup(int oldfd);
int dup2(int oldfd, int newfd);
pid_t fork(void);
int socketpair(int domain, int type, int protocol, int sv[2]);
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
int socket(int domain, int type, int protocol);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int listen(int sockfd, int backlog);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
int chdir(const char *path);
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
int fcntl(int fd, int cmd, ... / arg / );
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
pid_t waitpid(pid_t pid, int *wstatus, int options);
int kill(pid_t pid, int sig);
sighandler_t signal(int signum, sighandler_t handler);
int access(const char *pathname, int mode);
int stat(const char *pathname, struct stat *statbuf);
DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
*/

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    return 0;
}
