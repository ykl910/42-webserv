#!/bin/bash

PORT=8080
URL="http://localhost:8080"
RUN=0

if [ $# -eq 1 ]; then
    echo $1
    RUN=$1
fi

if [ $RUN -lt 10 ]; then
    ((RUN++))
    ./ddos.sh $RUN &
else
    exit 0
fi

if curl $URL; then
    while true; do
        curl $URL
    done
fi
