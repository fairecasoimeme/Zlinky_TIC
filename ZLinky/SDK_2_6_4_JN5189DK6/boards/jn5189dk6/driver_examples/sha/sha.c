/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_sha.h"

#include <string.h>

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define EXAMPLE_SHA (SHA0)
#define EXAMPLE_SHA_CLOCK (kCLOCK_Sha0)
#define EXAMPLE_SHA_RESET (kHASH_RST_SHIFT_RSTn)


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static const uint8_t s_Input[] = "Hello SHA-256 world.";

static uint8_t s_Digest[32] = {0x0U};

static const uint8_t s_DigestExpected[32] = {0xB3, 0x4F, 0xEF, 0x9B, 0xBA, 0xE4, 0xF9, 0x8C, 0xF3, 0xDE, 0xC5,
                                             0xAA, 0x16, 0xE7, 0xF8, 0xBF, 0x94, 0x8D, 0xE1, 0x0B, 0xC2, 0xE7,
                                             0x39, 0x4B, 0x6C, 0x92, 0xEB, 0xE8, 0xAA, 0x05, 0x71, 0x72};

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function.
 */
int main(void)
{
    status_t status;
    sha_ctx_t ctx;
    uint32_t i;
    size_t digestSize = sizeof(s_Digest);

    /* Init hardware */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    /* Ungate clock to SHA engine and reset it */
    CLOCK_EnableClock(EXAMPLE_SHA_CLOCK);
    RESET_PeripheralReset(EXAMPLE_SHA_RESET);

    /* Calculate, compare and print SHA-256 */

    PRINTF("SHA Peripheral Driver Example\r\n\r\n");

    PRINTF("Calculating SHA-256 digest.\r\n");
    PRINTF("Input: %s\r\n", (const char *)&(s_Input[0]));

    status = SHA_Init(EXAMPLE_SHA, &ctx, kSHA_Sha256);
    if (status != kStatus_Success)
    {
        PRINTF("Failed to initialize SHA module.\r\n");
        return 1;
    }

    status = SHA_Update(EXAMPLE_SHA, &ctx, s_Input, sizeof(s_Input) - 1U);
    if (status != kStatus_Success)
    {
        PRINTF("Failed to update SHA.\r\n");
        return 1;
    }

    status = SHA_Finish(EXAMPLE_SHA, &ctx, s_Digest, &digestSize);
    if (status != kStatus_Success)
    {
        PRINTF("Failed to finish SHA calculation.\r\n");
        return 1;
    }

    PRINTF("Output: ");
    for (i = 0U; i < digestSize; i++)
    {
        PRINTF("%02X", s_Digest[i]);
    }
    PRINTF("\r\n");

    if (memcmp(s_Digest, s_DigestExpected, sizeof(s_DigestExpected)) != 0)
    {
        PRINTF("Output does not match expected one.\r\n");
        return 1;
    }

    while (1)
    {
    }
}
