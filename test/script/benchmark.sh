#!/bin/bash

PROJECT_DIR=$(cd ../..; pwd)
TEST_DIR="$PROJECT_DIR/test"
LOG_DIR=$TEST_DIR/log

MULTIPLEXERS=(
    "select"
    "poll"
    "epoll"
)

HTTP_REQUEST=(
    "GET"
    "POST"
    "DELETE"
)

WEBSITE_PAGE=(
    "index.html"
)

mkdir -p "$LOG_DIR"
for multiplexer in "${MULTIPLEXERS[@]}"; do
    time ./webserv $multiplexer > "$LOG_DIR/$multiplexer.log"
done