#!/bin/bash
# usage: tests/robustness.sh   (à lancer depuis la racine du projet)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

head -c 100 /bin/ls > "$tmp/trunc100"      # tronqué après le header
head -c 64  /bin/ls > "$tmp/trunc64"       # header seul
head -c 10  /bin/ls > "$tmp/trunc10"       # tronqué dans le header
: > "$tmp/empty"                           # fichier vide
cp README.md "$tmp/not_elf"                # pas un ELF

fail=0
for f in "$tmp"/*; do
    ./binscope "$f" > /dev/null 2>&1
    rc=$?
    if [ "$rc" -ge 128 ]; then
        echo "CRASH ($rc) $(basename "$f")"
        fail=1
    elif [ "$rc" -eq 0 ]; then
        echo "FAIL (accepté à tort) $(basename "$f")"
        fail=1
    else
        echo "OK   $(basename "$f")"
    fi
done

./binscope /nexiste/pas > /dev/null 2>&1
if [ $? -eq 1 ]; then echo "OK   fichier inexistant"; else echo "FAIL fichier inexistant"; fail=1; fi

exit $fail