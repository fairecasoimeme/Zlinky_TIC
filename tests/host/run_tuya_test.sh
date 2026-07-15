#!/bin/sh
# Host-side test for bTuyaRequestPayloadValid() in TuyaSpecificCommandHandler.c.
# Extracts the REAL predicate (between its BEGIN/END markers) so the test
# tracks the source, and compiles it with test_tuya_payload.c.
#
# Usage:  sh tests/host/run_tuya_test.sh
set -e

ROOT=$(cd "$(dirname "$0")/../.." && pwd)
BUILD=$(mktemp -d)
trap 'rm -rf "$BUILD"' EXIT

# Minimal firmware base types so the extracted predicate compiles standalone.
cat > "$BUILD/jendefs.h" <<'EOF'
#include <stdint.h>
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint8_t  bool;
#define PUBLIC
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif
EOF

{
    printf '#include "jendefs.h"\n\n'
    awk '/BEGIN tuya-payload-validation/,/END tuya-payload-validation/' \
        "$ROOT/ZLinky/Source/TuyaSpecificCommandHandler.c"
} > "$BUILD/tuya_predicate.c"

if ! grep -q 'bTuyaRequestPayloadValid' "$BUILD/tuya_predicate.c"; then
    echo "ERROR: could not extract bTuyaRequestPayloadValid() from the source" >&2
    exit 2
fi

cc -std=gnu11 -I "$BUILD" -fsanitize=address -g -Wall \
   "$ROOT/tests/host/test_tuya_payload.c" "$BUILD/tuya_predicate.c" \
   -o "$BUILD/test_tuya"

"$BUILD/test_tuya"
