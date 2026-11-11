#!/bin/bash
set -e

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
if command -v root-config >/dev/null 2>&1; then
    exec make
fi
exec startmnxb11container.sh make
