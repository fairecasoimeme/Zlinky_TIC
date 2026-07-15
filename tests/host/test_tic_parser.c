/*
 * Host-side regression test for the Linky TIC parser
 * (Common/Source/app_uartlinky.c).
 *
 * It compiles and drives the REAL parser functions on a PC, feeding them
 * byte-by-byte exactly like APP_vProcessRxData* does, with the accumulation
 * buffers allocated on the heap so AddressSanitizer traps any out-of-bounds
 * access.
 *
 * Build & run:
 *   cc -I tests/host/stubs -fsanitize=address -g \
 *      tests/host/test_tic_parser.c Common/Source/app_uartlinky.c -o /tmp/test_tic
 *   /tmp/test_tic
 *
 * NOT part of the firmware build.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jendefs.h"

/* ---- stubs for the HW symbols referenced by app_uartlinky.c ---- */
#include "fsl_usart.h"
#include "ZQueue.h"

tszQueue APP_msgSerialRx;

void   USART_GetDefaultConfig(usart_config_t *c)          { if (c) memset(c, 0, sizeof(*c)); }
void   USART_Init(void *b, const usart_config_t *c, uint32 f) { (void)b; (void)c; (void)f; }
void   USART_Deinit(void *b)                              { (void)b; }
int    USART_SetBaudRate(void *b, uint32 r, uint32 f)     { (void)b; (void)r; (void)f; return kStatus_Success; }
uint32 USART_GetStatusFlags(void *b)                      { (void)b; return 0; }
void   USART_ClearStatusFlags(void *b, uint32 m)          { (void)b; (void)m; }
uint8  USART_ReadByte(void *b)                            { (void)b; return 0; }
uint32 CLOCK_GetFreq(int n)                               { (void)n; return 32000000u; }
bool   ZQ_bQueueSend(void *q, const void *it)             { (void)q; (void)it; return TRUE; }

/* ---- parser prototypes (from app_uartlinky.h) ---- */
bool bSL_ReadMessageStandard(uint16 u16MaxLength, uint8 *command, uint8 *date,
                             uint8 *value, uint8 *error, uint8 *posFinal,
                             uint8 u8Data, bool *lf);
bool bSL_ReadMessageHisto(uint16 u16MaxLength, uint8 *command, uint8 *value,
                          uint8 *error, uint8 u8Data, bool *lf);

/* Real buffer sizes as declared in ZLinky/Source/App_Linky.c */
#define SZ_COMMAND 128
#define SZ_DATE    128
#define SZ_VALUE   256
#define MAXLEN     270          /* MAX_PACKET_SIZE, as passed by the caller */

static int g_failures = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("  FAIL: %s\n", (msg)); g_failures++; } \
    else         { printf("  ok  : %s\n", (msg)); } \
} while (0)

/* ---------- Test 1: oversized label must not overflow the command buffer ---------- */
static void test_standard_label_not_overflowed(void)
{
    printf("[standard] oversized label (600 bytes, no separator)\n");
    uint8 *command = malloc(SZ_COMMAND);
    uint8 *date    = malloc(SZ_DATE);
    uint8 *value   = malloc(SZ_VALUE);
    uint8 error = 0, posFinal = 0;
    bool  lf = FALSE;

    /* LF (0x0A) resets the parser state, then the caller raises lf. */
    bSL_ReadMessageStandard(MAXLEN, command, date, value, &error, &posFinal, 0x0A, &lf);
    lf = TRUE;
    /* 600 label bytes, no 0x09 separator: pos stays 0, writes into command. */
    for (int i = 0; i < 600; i++)
        bSL_ReadMessageStandard(MAXLEN, command, date, value, &error, &posFinal, 'A', &lf);

    CHECK(command[SZ_COMMAND - 1] == '\0', "command stays NUL-terminated in-bounds");
    free(command); free(date); free(value);
}

/* ---------- Test 2: oversized value must not overflow the value buffer ---------- */
static void test_standard_value_not_overflowed(void)
{
    printf("[standard] oversized value (600 bytes, no separator)\n");
    uint8 *command = malloc(SZ_COMMAND);
    uint8 *date    = malloc(SZ_DATE);
    uint8 *value   = malloc(SZ_VALUE);
    uint8 error = 0, posFinal = 0;
    bool  lf = FALSE;

    bSL_ReadMessageStandard(MAXLEN, command, date, value, &error, &posFinal, 0x0A, &lf);
    lf = TRUE;
    bSL_ReadMessageStandard(MAXLEN, command, date, value, &error, &posFinal, 'E', &lf); /* label */
    bSL_ReadMessageStandard(MAXLEN, command, date, value, &error, &posFinal, 0x09, &lf); /* pos->1 */
    bSL_ReadMessageStandard(MAXLEN, command, date, value, &error, &posFinal, 'D', &lf); /* date  */
    bSL_ReadMessageStandard(MAXLEN, command, date, value, &error, &posFinal, 0x09, &lf); /* pos->2 */
    for (int i = 0; i < 600; i++)
        bSL_ReadMessageStandard(MAXLEN, command, date, value, &error, &posFinal, 'B', &lf); /* value */

    CHECK(value[SZ_VALUE - 1] == '\0', "value stays NUL-terminated in-bounds");
    free(command); free(date); free(value);
}

/* ---------- Test 3: oversized label/value in historique mode ---------- */
static void test_histo_not_overflowed(void)
{
    printf("[histo] oversized label then oversized value\n");
    uint8 *command = malloc(SZ_COMMAND);
    uint8 *value   = malloc(SZ_VALUE);
    uint8 error = 0;
    bool  lf = FALSE;

    bSL_ReadMessageHisto(MAXLEN, command, value, &error, 0x0A, &lf);
    lf = TRUE;
    for (int i = 0; i < 600; i++)                                   /* label, pos 0 */
        bSL_ReadMessageHisto(MAXLEN, command, value, &error, 'A', &lf);
    bSL_ReadMessageHisto(MAXLEN, command, value, &error, 0x20, &lf); /* pos->1 */
    for (int i = 0; i < 600; i++)                                   /* value, pos 1 */
        bSL_ReadMessageHisto(MAXLEN, command, value, &error, 'B', &lf);

    CHECK(command[SZ_COMMAND - 1] == '\0', "command stays NUL-terminated in-bounds");
    CHECK(value[SZ_VALUE - 1] == '\0', "value stays NUL-terminated in-bounds");
    free(command); free(value);
}

/* ---------- Test 4: a well-formed historique group must still parse (no regression) ---------- */
static void test_histo_valid_frame_still_parses(void)
{
    printf("[histo] well-formed frame 'ADCO 12 <crc>' parses with valid CRC\n");
    uint8 *command = malloc(SZ_COMMAND);
    uint8 *value   = malloc(SZ_VALUE);
    uint8 error = 0;
    bool  lf = FALSE;
    bool  got_true = FALSE;

    /* Frame: label "ADCO", SP, value "12", SP, checksum, CR.
     * Historique CRC accumulates label + first separator + value:
     *   0x41+0x44+0x43+0x4F = 0x17 (mod 256), +0x20 = 0x37, +0x31 = 0x68, +0x32 = 0x9A.
     * Checksum char = (0x9A & 0x3F) + 0x20 = 0x1A + 0x20 = 0x3A. */
    const uint8 frame[] = { 0x0A, 'A','D','C','O', 0x20, '1','2', 0x20, 0x3A, 0x0D };
    for (size_t i = 0; i < sizeof(frame); i++) {
        if (frame[i] == 0x0A) {
            bSL_ReadMessageHisto(MAXLEN, command, value, &error, 0x0A, &lf);
            lf = TRUE;                 /* caller raises lf after the LF */
        } else if (bSL_ReadMessageHisto(MAXLEN, command, value, &error, frame[i], &lf)) {
            got_true = TRUE;
        }
    }

    CHECK(got_true == TRUE, "frame reported valid (CRC matched)");
    CHECK(strcmp((char *)command, "ADCO") == 0, "command == \"ADCO\"");
    CHECK(strcmp((char *)value, "12") == 0, "value == \"12\"");
    free(command); free(value);
}

int main(void)
{
    test_standard_label_not_overflowed();
    test_standard_value_not_overflowed();
    test_histo_not_overflowed();
    test_histo_valid_frame_still_parses();

    if (g_failures) { printf("\n%d CHECK failure(s)\n", g_failures); return 1; }
    printf("\nAll TIC parser checks passed.\n");
    return 0;
}
