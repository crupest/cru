#!/usr/bin/env sh

if [[ $1 == *.js ]]; then
    exec node -- "$@"
fi

exec "$@"
