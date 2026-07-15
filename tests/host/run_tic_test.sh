#!/bin/sh
# Host-side regression test runner for the Linky TIC parser.
# Compiles the REAL Common/Source/app_uartlinky.c against lightweight stubs,
# under AddressSanitizer, and drives it from test_tic_parser.c.
#
# The .c under test is copied to an isolated directory so that its quoted
# #include "..." directives resolve against tests/host/stubs (and not the
# real, SDK-dependent sibling headers).
#
# Usage:  sh tests/host/run_tic_test.sh
set -e

ROOT=$(cd "$(dirname "$0")/../.." && pwd)
STUBS="$ROOT/tests/host/stubs"
BUILD=$(mktemp -d)
trap 'rm -rf "$BUILD"' EXIT

# Copy the unit under test AND its own header (which carries the LK_TIC_*_SIZE
# constants) into an isolated dir, so quoted includes resolve there / in stubs.
cp "$ROOT/Common/Source/app_uartlinky.c" "$BUILD/app_uartlinky.c"
cp "$ROOT/Common/Source/app_uartlinky.h" "$BUILD/app_uartlinky.h"

cc -std=gnu11 -I "$STUBS" -include string.h -fsanitize=address -g -Wall -Wextra \
   -Wno-unused-parameter -Wno-sign-compare \
   "$ROOT/tests/host/test_tic_parser.c" \
   "$BUILD/app_uartlinky.c" \
   -o "$BUILD/test_tic"

"$BUILD/test_tic"
