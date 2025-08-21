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

    local multiplexers=(
        "select"
        "poll"
        "epoll"
    )

    for multiplexer in "${multiplexers[@]}"; do
        ./ubuntu_tester
        ./ubuntu_cgi_tester
    done

    # SELECT
    # POLL
    # EPOLL
    return;
}

main "$@"