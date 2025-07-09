#include "../include/Signal.hpp"

void    Signal::sigHandler(int signum) {
    switch (signum) {
        // Ctrl-C
        case SIGINT:
            std::cout << BOLD WHITE << "SIGINT catched" << std::endl;
            break;

        // Ctrl-Z
        case SIGSTOP:
            std::cout << BOLD WHITE << "SIGSTOP catched" << std::endl;
            break;

        // Ctrl-
        case SIGQUIT:
            std::cout << BOLD WHITE << "SIGQUIT catched" << std::endl;
            break;
    }
}

Signal::Signal() {
    if (signal(SIGINT, this->sigHandler) == SIG_ERR
        || signal(SIGSTOP, this->sigHandler) == SIG_ERR
        || signal(SIGQUIT, this->sigHandler) == SIG_ERR)
        throw std::runtime_error("Error: signal init.");
}

Signal::~Signal() {

}
