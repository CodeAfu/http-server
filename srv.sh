#!/bin/bash

VERBOSE=0
[[ "$1" == "-v" || "$1" == "--verbose" ]] && VERBOSE=1

while true; do
    # [[ $VERBOSE -eq 0 ]] && clear
    clear
    if [[ $VERBOSE -eq 1 ]]; then
        make build && ./build/http-server &
    else
        OUTPUT=$(make build 2>&1)
        if [[ $? -ne 0 ]]; then
            echo "$OUTPUT"
            inotifywait -e modify -r src/ include/ &>/dev/null
            continue
        fi
        ./build/http-server &
    fi
    PID=$!
    inotifywait -e modify -r src/ include/ &>/dev/null
    kill $PID
    wait $PID 2>/dev/null
done
