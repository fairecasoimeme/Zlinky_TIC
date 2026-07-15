/*
 * Host-side test for bTuyaRequestPayloadValid() in
 * ZLinky/Source/TuyaSpecificCommandHandler.c.
 *
 * That predicate is the guard added to eCLD_TuyaCommandRspSend so that a
 * truncated or oversized (untrusted, over-radio) Tuya TY_DATA_REQUEST is
 * rejected before the handler indexes the payload at fixed offsets and sizes
 * a stack array from its length. The runner extracts the REAL predicate from
 * the source so the test tracks it.
 *
 * Build & run:  sh tests/host/run_tuya_test.sh
 * NOT part of the firmware build.
 */
#include <stdio.h>
#include "jendefs.h"

bool bTuyaRequestPayloadValid(uint16 u16PayloadSize, uint8 u8DataPoint);

static int fails = 0;
#define CHECK(c, m) do { \
    if (!(c)) { printf("  FAIL: %s\n", (m)); fails++; } \
    else      { printf("  ok  : %s\n", (m)); } \
} while (0)

int main(void)
{
    printf("[tuya] truncated frames are rejected\n");
    CHECK(bTuyaRequestPayloadValid(0, 0x11) == FALSE, "size 0 rejected");
    CHECK(bTuyaRequestPayloadValid(3, 0x00) == FALSE, "size 3 (would underflow the VLA) rejected");
    CHECK(bTuyaRequestPayloadValid(8, 0x00) == FALSE, "size 8 (header incomplete) rejected");
    CHECK(bTuyaRequestPayloadValid(9, 0x00) == TRUE,  "size 9 (header complete) accepted");

    printf("[tuya] data point 0x11 needs 13 bytes\n");
    CHECK(bTuyaRequestPayloadValid(12, 0x11) == FALSE, "DP 0x11 with 12 bytes rejected");
    CHECK(bTuyaRequestPayloadValid(13, 0x11) == TRUE,  "DP 0x11 with 13 bytes accepted");

    printf("[tuya] data point 0x12 needs 21 bytes\n");
    CHECK(bTuyaRequestPayloadValid(20, 0x12) == FALSE, "DP 0x12 with 20 bytes rejected");
    CHECK(bTuyaRequestPayloadValid(21, 0x12) == TRUE,  "DP 0x12 with 21 bytes accepted");

    printf("[tuya] oversized frames are rejected (bounds the stack array)\n");
    CHECK(bTuyaRequestPayloadValid(129, 0x11) == FALSE, "size 129 rejected");
    CHECK(bTuyaRequestPayloadValid(65535, 0x12) == FALSE, "size 65535 rejected");

    if (fails) { printf("\n%d CHECK failure(s)\n", fails); return 1; }
    printf("\nAll Tuya payload-length checks passed.\n");
    return 0;
}
