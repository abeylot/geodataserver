#!/bin/bash
# Usage: tests/png_tiles.sh [base_url]   (server must be running)
BASE=${1:-http://127.0.0.1:8081}
fail=0
check() { if [ "$2" != "$3" ]; then echo "FAIL $1: got '$2' expected '$3'"; fail=1; else echo "ok   $1"; fi; }

# tiles: dense city z14, coastline z8, empty ocean z3
for tile in 14/8000/5800 8/124/90 3/2/3; do
    out=$(mktemp)
    curl -s -o "$out" -D "$out.h" "$BASE/$tile.png"
    check "$tile magic" "$(head -c 8 "$out" | xxd -p)" "89504e470d0a1a0a"
    check "$tile size" "$(xxd -s 16 -l 8 -p "$out")" "0000010000000100"
    check "$tile type" "$(grep -i '^content-type' "$out.h" | tr -d '\r' | awk '{print $2}')" "image/png"
    rm -f "$out" "$out.h"
done

check "out of range" "$(curl -s -o /dev/null -w '%{http_code}' "$BASE/3/9/9.png")" "404"

etag=$(curl -s -D - -o /dev/null "$BASE/3/2/3.png" | tr -d '\r' | grep -i '^etag' | cut -d' ' -f2)
check "304" "$(curl -s -o /dev/null -w '%{http_code}' -H "If-None-Match: $etag" "$BASE/3/2/3.png")" "304"
exit $fail
