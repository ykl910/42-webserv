#include "../include/Signal.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>

void    Signal::sigHandler(int signum) {
    switch (signum) {
        // Ctrl-C
        case SIGINT:
            std::cout << BOLD WHITE << "\nSIGINT catched" << DEFAULT << std::endl;
            exit(0);
            break;

        // Ctrl-Z
        // case SIGSTOP:
        //     std::cout << BOLD WHITE << "\nSIGSTOP catched" << DEFAULT << std::endl;
        //     break;

        // Ctrl-
        case SIGQUIT:
            std::cout << BOLD WHITE << "\nSIGQUIT catched" << DEFAULT << std::endl;
            break;
    }
}

Signal::Signal() {
    if (signal(SIGINT, this->sigHandler) == SIG_ERR
        || signal(SIGQUIT, this->sigHandler) == SIG_ERR) {
        throw std::runtime_error("Error: signal init.");
    }
}

Signal::~Signal() {

}
