#!/bin/bash
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
head -c 100 /bin/ls > "$tmp/bad"

for f in /bin/ls /usr/bin/python3 "$tmp/bad" tests/exec; do
    valgrind --error-exitcode=99 --leak-check=full ./binscope "$f" > /dev/null 2>&1
    if [ $? -eq 99 ]; then
        echo "FAIL valgrind $f"
        exit 1
    fi
    echo "OK   valgrind $f"
done