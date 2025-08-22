#!/bin/bash

set -e

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
}

run_test() {

    local multiplexers=(
        "select"
        "poll"
        "epoll"
    )

    for multiplexer in "${multiplexers[@]}"; do

        # Subject test
        ./ubuntu_tester
        ./ubuntu_cgi_tester

        # Siege

        # Perso

    done

    return;
}

main "$@"