/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_clock.h"
#include "board.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

/*******************************************************************************
 * Local Prototypes
 ******************************************************************************/

/*****************************************************************************
 * Local functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize debug console. */
status_t BOARD_InitDebugConsole(void)
{
    status_t result;

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;

    result =
        DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);

#ifndef RTL_SIMU_ON_ES2
    CLOCK_uDelay(500);
#endif

    return result;
}

/* Dummy functions, added to enable LowPower module to link */
void BOARD_DbgDiagEnable(void)
{
}
void BOARD_DbgLpIoSet(int pinid, int val)
{
    (void)(pinid);
    (void)(val);
}
