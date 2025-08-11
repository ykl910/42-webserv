#include "../include/Signal.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>

/*
The master process of nginx can handle the following signals:

SIGINT, SIGTERM
    Shut down quickly.
SIGHUP
    Reload configuration, start the new worker process with a new configuration,
    and gracefully shut down old worker processes.
SIGQUIT
    Shut down gracefully.
SIGUSR1
    Reopen log files.
SIGUSR2
    Upgrade the nginx executable on the fly.
SIGWINCH
    Shut down worker processes gracefully.

While there is no need to explicitly control worker processes normally, they
support some signals too:

SIGTERM
    Shut down quickly.
SIGQUIT
    Shut down gracefully.
SIGUSR1
    Reopen log files.
*/

void    signalHandler(int signum)
{
    switch (signum) {
        // Ctrl-C
        case SIGINT:
            std::cout << BOLD WHITE << "\nSIGINT catched" << DEFAULT
            << std::endl;
            exit(EXIT_SUCCESS);
            break;

        // Ctrl-
        case SIGQUIT:
            std::cout << BOLD WHITE << "\nSIGQUIT catched" << DEFAULT
            << std::endl;
            break;
    }
}

void    initSignalHandler(void)
{
    if (signal(SIGINT, signalHandler) == SIG_ERR
        || signal(SIGQUIT, signalHandler) == SIG_ERR)
        throw std::runtime_error("Error: signal init.");
}
