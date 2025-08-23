#!/bin/bash

URL="http://localhost:8080"
RUN=0

if [ $# -eq 1 ]; then
    RUN=$1
fi

if [ $RUN -lt 10 ]; then
    ((RUN++))
    ./dos.sh $RUN &
else
    exit 0
fi

if curl $URL; then
    while true; do
        if ! curl $URL; then
            exit 0
        fi
    done
fi
