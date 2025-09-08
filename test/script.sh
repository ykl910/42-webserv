#!/bin/bash

set -e

header="\
░█░░▒█▒██▀░██▄░▄▀▀▒██▀▒█▀▄░█▒█
░▀▄▀▄▀░█▄▄▒█▄█▒▄██░█▄▄░█▀▄░▀▄▀"

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

project_dir=$(cd .. && pwd)
test_dir="$project_dir/test"
log_dir="$test_dir/log"

# host:port
domains=(
    "post42.net"
    "webserv.net"
)

host="localhost"

ports=(
    "8080"
    "4040"
)

multiplexers=(
    "select"
    "poll"
    "epoll"
)

main()
{
    echo -e "$BOLD$BLUE$header$DEFAULT\n"

    if [ $# -eq 1 ]; then
        case $1 in
            "subject")
            run_subject_test
            ;;
            "siege")
            run_siege_test
            ;;
            "curl")
            run_curl_test
            ;;
            default)
            print_usage
            ;;
        esac
    elif [ $# -eq 0 ]; then
        setup_test
        run_all_test
    else
        print_usage
    fi
    exit 0
}

print_usage()
{
    echo -e "$BOLD${WHITE}usage: ${DEFAULT}./script.sh [test name]"
}

setup_test()
{
    # TMUX
    # SIEGE
    return;
}

run_all_test()
{
    run_subject_test
    run_siege_test
    run_curl_test
}

run_subject_test()
{
    if [ ! -f "external/subject/ubuntu_tester" ]; then
        curl -O https://cdn.intra.42.fr/document/document/35869/ubuntu_tester
        chmod 755 "external/subject/ubuntu_tester"
    fi

    if [ ! -f "external/subject/ubuntu_cgi_tester" ]; then
        curl -O https://cdn.intra.42.fr/document/document/35867/ubuntu_cgi_tester
        chmod 755 "external/subject/ubuntu_cgi_tester"
    fi
    # Welcome in this little webserver tester.
    # Passing the test here is the minimum before going to an evaluation.
    # press enter to continue

    # THIS TEST IS NOT MEANT TO BE THE ONLY TEST IN THE EVALUATION!!!
    # press enter to continue

    # Before starting please follow the next few steps (files content can be anything and will be shown to you by the test):
    # - Download the cgi_test executable on the host
    # - Create a directory YoupiBanane with:
    #         -a file name youpi.bad_extension
    #         -a file name youpi.bla
    #         -a sub directory called nop
    #                 -a file name youpi.bad_extension in nop
    #                 -a file name other.pouic in nop
    #         -a sub directory called Yeah
    #                 -a file name not_happy.bad_extension in Yeah
    mkdir -p $test_dir/YoupiBanane
    touch $test_dir/YoupiBanane/youpi.bad_extension
    touch $test_dir/YoupiBanane/youpi.bla

    mkdir -p $test_dir/YoupiBanane/nop
    touch $test_dir/YoupiBanane/nop/youpi.bad_extension
    touch $test_dir/YoupiBanane/nop/other.pouic

    mkdir -p $test_dir/YoupiBanane/Yeah
    touch $test_dir/YoupiBanane/Yeah/not_happy.bad_extension
    # press enter to continue

    # Setup the configuration file as follow:
    # - / must answer to GET request ONLY
    # - /put_test/* must answer to PUT request and save files to a directory of your choice
    # - any file with .bla as extension must answer to POST request by calling the cgi_test executable
    # - /post_body must answer anything to POST request with a maxBody of 100
    # - /directory/ must answer to GET request and the root of it would be the repository YoupiBanane and if no file are requested, it should search for youpi.bad_extension files
    echo \
"server:
    listen 8080
    host localhost
    server_name subject.com
    client_max_body_size 10m
    error_page:

    location \\:
" > test.conf
    # press enter to continue


    # Before starting please verify that the server is launched
    bash -c "cd $project_dir && ./webserv"
    # press enter to continue

    rm -r YoupiBanane test.conf
}

run_siege_test()
{
    local client_number=100

    echo -e "${BOLD}${ITALIC}${YELLOW}Siege test${DEFAULT}"
    mkdir -p $log_dir/siege
    pushd .. > /dev/null
    for multiplexer in "${multiplexers[@]}";
    do
        for port in "${ports[@]}";
        do
            ./webserv $multiplexer &> /dev/null &
            webserv_pid=$!
            siege -c 255 -t 2s http://$host:$port > "$log_dir/siege/$multiplexer.log"
            kill $webserv_pid
        done
    done
    popd > /dev/null
}

run_curl_test()
{
    local webserv_pid
    echo -e "${BOLD}${ITALIC}${YELLOW}cURL test${DEFAULT}"

    mkdir -p ../log
    pushd .. > /dev/null
    for multiplexer in "${multiplexers[@]}";
    do
        ./webserv $multiplexer &
        webserv_pid=$!
        for domain in "${domains[@]}";
        do
            # Empty request
            # curl -d http $host:$port

            # GET
            curl -X GET http://$host:$port

            # POST
            curl -X POST http://$host:$port

            # DELETE
            curl -X DELETE http://$host:$port
        done
        kill $webserv_pid
    done
    popd > dev/null
}

main "$@"