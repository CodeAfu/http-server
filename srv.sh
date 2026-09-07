#!/bin/bash

VERBOSE=0
[[ "$1" == "-v" || "$1" == "--verbose" ]] && VERBOSE=1

PID=""

stop_server() {
    if [[ -n "$PID" ]] && kill -0 "$PID" 2>/dev/null; then
        kill "$PID" 2>/dev/null
        wait "$PID" 2>/dev/null
    fi

    PID=""
}

handle_exit() {
    trap - EXIT
    stop_server
    exit 130
}

trap handle_exit INT TERM
trap stop_server EXIT

while true; do
    clear

    if [[ $VERBOSE -eq 1 ]]; then
        if ! make build; then
            inotifywait -e modify -r src/ include/ &>/dev/null
            continue
        fi
    else
        OUTPUT=$(make build 2>&1)
        if [[ $? -ne 0 ]]; then
            echo "$OUTPUT"
            inotifywait -e modify -r src/ include/ &>/dev/null
            continue
        fi
    fi

    ENV=development ./build/http-server &
    PID=$!

    inotifywait -e modify -r src/ include/ &>/dev/null
    stop_server
done
