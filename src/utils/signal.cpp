#include "../../include/signal.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>

int g_signal;

void    signalHandler(int signum)
{
    switch (signum) {
        // Ctrl-C
        case SIGINT:
            std::cout << BOLD YELLOW << "\nSIGINT "
            << BOLD WHITE << "catched.\n" << DEFAULT;
            g_signal = SIGINT;
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
        || signal(SIGQUIT, signalHandler) == SIG_ERR
        || signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        throw std::runtime_error("Error: signal init.");
}
