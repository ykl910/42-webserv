#!/bin/bash

set -e

# Text formating
DEFAULT="\033[0m"
BOLD="\033[1m"
ITALIC="\033[3m"
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
MAGENTA="\033[0;35m"
CYAN="\033[0;36m"
WHITE="\033[0;37m"

# host:port
DOMAIN=(
    "post42.net"
    "webserv.net"
)
HOST="localhost"
PORT="8080"

main() {
    if [ $# -eq 1 ]; then
        case $1 in
            "curl")
            test_curl
            ;;
            "siege")
            test_siege
            ;;
            "subject")
            test_subject
            ;;
        esac
        run_test
    elif [ $# -eq 0 ]; then
        setup_test
    else
        print_usage
    fi
    exit 0
}

print_usage() {
    echo "usage: ./script.sh [test name]"
}

setup_test() {
    # TMUX
    # SIEGE
    return;
}

test_curl() {
    echo -e "$BOLD$ITALIC${YELLOW}cURL test$DEFAULT"

    for domain in "${DOMAIN[@]}"; do
        curl -d http $HOST:$PORT
    done
}

test_siege() {
    local client_number=100

    mkdir -p ../
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