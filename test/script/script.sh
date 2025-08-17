#!/bin/bash

main() {
    if [ $# -gt 0 ]; then
        run_test
    else
        setup_test
    fi
}

setup_test() {
    # TMUX

    # SIEGE

    return;
}

run_test() {
    # SELECT
    # POLL
    # EPOLL
    return;
}

main "$@"