#!/bin/sh
# Host-side regression test for trim() in ZLinky/Source/App_Linky.c.
# Extracts the REAL trim() from App_Linky.c (so the test always tracks the
# source) and compiles it under AddressSanitizer with test_trim.c.
#
# Usage:  sh tests/host/run_trim_test.sh
set -e

ROOT=$(cd "$(dirname "$0")/../.." && pwd)
BUILD=$(mktemp -d)
trap 'rm -rf "$BUILD"' EXIT

{
    printf '#include <ctype.h>\n#include <string.h>\n\n'
    awk '/^char\*  trim/,/^}/' "$ROOT/ZLinky/Source/App_Linky.c"
} > "$BUILD/trim_extracted.c"

if ! grep -q 'trim' "$BUILD/trim_extracted.c"; then
    echo "ERROR: could not extract trim() from App_Linky.c" >&2
    exit 2
fi

cc -std=gnu11 -fsanitize=address -g -Wall \
   "$ROOT/tests/host/test_trim.c" "$BUILD/trim_extracted.c" \
   -o "$BUILD/test_trim"

"$BUILD/test_trim"
