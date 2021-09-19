#!/bin/sh
#
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo -e "${GREEN}** Running **"

while true
do

    inotifywait ./* -q -e modify
    echo
    echo
    echo -e "${GREEN}** Running **${NC}"
    echo
    echo
    make --no-print-directory run
done
