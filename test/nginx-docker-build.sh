#!/bin/bash

set -e

docker images > outfile

if ! grep nginx outfile; then
    docker pull nginx
fi

docker ps > outfile

if grep nginx outfile > /dev/null; then
    while IFS= read -r line; do
        if grep nginx <<< $line > /dev/null; then
            id=$(awk '{print $1}'<<< $line)
            docker stop $id
        fi
    done < outfile
fi

if [ $# -gt 0 ] && [ $1 == "run" ]; then
    docker run \
        -p 4241:4241 \
        -d nginx \
        -v $(pwd)/nginx.conf:/etc/nginx/nginx.conf:ro \
        -v /media/algadea/extreme-ssd/dev/42/webserv/www/webserv.net:/media/algadea/extreme-ssd/dev/42/webserv/www/webserv.net:ro \
        nginx
fi

rm outfile