#!/bin/bash
# usage: tests/compare_dynsym.sh <binary>
bin=${1:?usage: $0 <binary>}

mine=$(./binscope "$bin" | sed -n '/^\.dynsym$/,$p' | sed 1,2d \
    | sed 's/\[ *\([0-9]*\)\]/\1/' \
    | awk '{print $1, substr($2,3), $3, $4, $5, $6, $7}')

ref=$(readelf -W --dyn-syms "$bin" \
    | gawk '$1 ~ /^[0-9]+:$/ {n=$1; sub(":","",n); name=$8; sub(/@.*/,"",name);
            size=$3; if (size ~ /^0x/) size=strtonum(size);
            print n, $2, size, $4, $5, $7, name}')

if diff <(echo "$mine") <(echo "$ref") > /dev/null; then
    echo "OK   $bin"
else
    echo "FAIL $bin"
    exit 1
fi