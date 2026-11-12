#!/bin/bash
set -e

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
if [[ ! -x ./main ]]; then
    printf 'The analysis executable is missing. Build it with bash scripts/buildproject.sh first.\n' >&2
    exit 1
fi

if command -v root-config >/dev/null 2>&1; then
    exec ./main
fi
exec startmnxb11container.sh ./main
