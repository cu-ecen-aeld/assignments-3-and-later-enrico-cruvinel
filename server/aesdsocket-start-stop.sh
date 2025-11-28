#!/bin/sh

AESDSOCKET="/usr/bin/aesdsocket"

if [ "$1" = "start" ]; then
    start-stop-daemon --start --exec ${AESDSOCKET} -- -d
    exit 0
fi

if [ "$1" = "stop" ]; then
    start-stop-daemon --stop --exec ${AESDSOCKET} --signal TERM
    exit 0
fi

echo "Usage: $0 {start|stop}"
exit 1
