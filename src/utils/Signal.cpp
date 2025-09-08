#include "../../include/Signal.hpp"
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

int g_signal;

void    signalHandler(int signum)
{
    switch (signum) {
        // Ctrl-C
        case SIGINT:
            std::cout << BOLD YELLOW << "\nSIGINT "
            << BOLD WHITE << "catched.\n" << DEFAULT;
            g_signal = SIGINT;
            exit(0);
            break;

        // Ctrl-
        case SIGQUIT:
            std::cout << BOLD YELLOW << "\nSIGQUIT "
            << BOLD WHITE << "catched.\n" << DEFAULT;
            break;
    }
}

void    initSignalHandler(void)
{
    g_signal = 0;
    if (signal(SIGINT, signalHandler) == SIG_ERR
        || signal(SIGQUIT, signalHandler) == SIG_ERR)
        throw std::runtime_error("Error: signal init.");
}
