#!/bin/sh
# Host-side test for iFindReportSlot() in ZLinky/Source/app_reporting.c.
# Extracts the REAL helper (between its BEGIN/END markers) and compiles it
# against a minimal struct carrying only the fields the helper reads.
#
# Usage:  sh tests/host/run_report_test.sh
set -e

ROOT=$(cd "$(dirname "$0")/../.." && pwd)
BUILD=$(mktemp -d)
trap 'rm -rf "$BUILD"' EXIT

# Minimal environment: the two struct fields the helper reads, the table size,
# and PRIVATE made external so the test can link against the extracted helper.
cat > "$BUILD/report_types.h" <<'EOF'
#include <stdint.h>
typedef uint16_t uint16;
#define PRIVATE
#define ZCL_NUMBER_OF_REPORTS 50
typedef struct { uint16 u16AttributeEnum; } tsAttrRec_min;
typedef struct { uint16 u16ClusterID; tsAttrRec_min sAttributeReportingConfigurationRecord; } tsReports_min;
extern tsReports_min asSavedReports[ZCL_NUMBER_OF_REPORTS];
EOF

{
    printf '#include "report_types.h"\n\n'
    awk '/BEGIN report-slot-search/,/END report-slot-search/' \
        "$ROOT/ZLinky/Source/app_reporting.c"
} > "$BUILD/report_slot.c"

if ! grep -q 'iFindReportSlot' "$BUILD/report_slot.c"; then
    echo "ERROR: could not extract iFindReportSlot() from app_reporting.c" >&2
    exit 2
fi

cc -std=gnu11 -I "$BUILD" -fsanitize=address -g -Wall \
   "$ROOT/tests/host/test_report_slot.c" "$BUILD/report_slot.c" \
   -o "$BUILD/test_report"

"$BUILD/test_report"
