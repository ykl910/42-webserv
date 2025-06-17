/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: algadea <algadea@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 20:10:23 by algadea           #+#    #+#             */
/*   Updated: 2025/06/17 14:23:44by algadea          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"

/*
int         execve(const char *pathname, char *const argv[], char *const envp[]);
int         pipe(int pipefd[2]);
char        *strerror(int errnum);
const char  *gai_strerror(int errcode);
int dup(int oldfd);
int dup2(int oldfd, int newfd);
*/
errno
fork
socketpair
htons
htonl
ntohs
ntohl
select
poll
epoll (epoll_create, epoll_ctl, epoll_wait)
kqueue (kqueue, kevent)
socket
accept
listen
send
recv
chdir
bind
connect
getaddrinfo
freeaddrinfo
setsockopt
getsockname
getprotobyname
fcntl
close
read
write
waitpid,
kill
signal
access
stat
open
opendir
readdir
closedir

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    return 0;
}