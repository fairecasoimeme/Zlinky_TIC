/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_ctimer.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CTIMER CTIMER0
#define CTIMER_MAT0_OUT kCTIMER_Match_0
#define CTIMER_MAT1_OUT kCTIMER_Match_3
#define CTIMER_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#define LED_PORT_0 0
#define LED_PIN_0 0
#define LED_PORT_1 0
#define LED_PIN_1 3
#define BOARD_HAS_NO_CTIMER_OUTPUT_PIN_CONNECTED_TO_LED 1
#define CTIMER_EMT0_OUT kCTIMER_External_Match_0
#define CTIMER_EMT1_OUT kCTIMER_External_Match_3

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void ctimer_match0_callback(uint32_t flags);
void ctimer_match1_callback(uint32_t flags);

/* Array of function pointers for callback for each channel */
ctimer_callback_t ctimer_callback_table[] = {
    ctimer_match0_callback, NULL, NULL, ctimer_match1_callback, NULL, NULL, NULL, NULL};


/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Match Configuration for Channel 0 */
static ctimer_match_config_t matchConfig0;
/* Match Configuration for Channel 1 */
static ctimer_match_config_t matchConfig1;

/*******************************************************************************
 * Code
 ******************************************************************************/

void ctimer_match1_callback(uint32_t flags)
{
    static uint32_t count            = 0;
    static uint32_t matchUpdateCount = 8;

    if (++count > matchUpdateCount)
    {
        count = 0;
        matchConfig1.matchValue >>= 1;
        matchUpdateCount <<= 1;
        if (matchUpdateCount == (1 << 8))
        {
            matchUpdateCount        = 8;
            matchConfig1.matchValue = CTIMER_CLK_FREQ / 2;
        }
        CTIMER_SetupMatch(CTIMER, CTIMER_MAT1_OUT, &matchConfig1);
    }
#if defined(BOARD_HAS_NO_CTIMER_OUTPUT_PIN_CONNECTED_TO_LED)
    /* No timer match output pin connected to a LED
    * toggle LED manually according to match status
    */
    if (CTIMER_GetOutputMatchStatus(CTIMER, CTIMER_EMT1_OUT))
    {
        LED_RED2_ON();
    }
    else
    {
        LED_RED2_OFF();
    }
#endif
}

void ctimer_match0_callback(uint32_t flags)
{
    static uint32_t count            = 0;
    static uint32_t matchUpdateCount = 8;

    if (++count > matchUpdateCount)
    {
        count = 0;
        matchConfig0.matchValue >>= 1;
        matchUpdateCount <<= 1;
        if (matchUpdateCount == (1 << 8))
        {
            matchUpdateCount        = 8;
            matchConfig0.matchValue = CTIMER_CLK_FREQ / 2;
        }
        CTIMER_SetupMatch(CTIMER, CTIMER_MAT0_OUT, &matchConfig0);
    }
#if defined(BOARD_HAS_NO_CTIMER_OUTPUT_PIN_CONNECTED_TO_LED)
    /* No timer match output pin connected to a LED
    * toggle LED manually according to match status
    */
    if (CTIMER_GetOutputMatchStatus(CTIMER, CTIMER_EMT0_OUT))
    {
        LED_RED1_ON();
    }
    else
    {
        LED_RED1_OFF();
    }
#endif
}

/*!
 * @brief Main function
 */
int main(void)
{
    ctimer_config_t config;

    /* Init hardware*/
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    CLOCK_EnableAPBBridge();
    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

#if defined(BOARD_HAS_NO_CTIMER_OUTPUT_PIN_CONNECTED_TO_LED)
    LED_RED1_INIT(LOGIC_LED_OFF);
    LED_RED2_INIT(LOGIC_LED_OFF);
#endif

    PRINTF("CTimer match example to toggle the output. \r\n");
    PRINTF("This example uses interrupt to change the match period. \r\n");

    CTIMER_GetDefaultConfig(&config);

    CTIMER_Init(CTIMER, &config);

    /* Configuration 0 */
    matchConfig0.enableCounterReset = true;
    matchConfig0.enableCounterStop  = false;
    matchConfig0.matchValue         = CTIMER_CLK_FREQ / 2;
    matchConfig0.outControl         = kCTIMER_Output_Toggle;
    matchConfig0.outPinInitState    = false;
    matchConfig0.enableInterrupt    = true;

    /* Configuration 1 */
    matchConfig1.enableCounterReset = true;
    matchConfig1.enableCounterStop  = false;
    matchConfig1.matchValue         = CTIMER_CLK_FREQ / 2;
    matchConfig1.outControl         = kCTIMER_Output_Toggle;
    matchConfig1.outPinInitState    = true;
    matchConfig1.enableInterrupt    = true;

    CTIMER_RegisterCallBack(CTIMER, &ctimer_callback_table[0], kCTIMER_MultipleCallback);
    CTIMER_SetupMatch(CTIMER, CTIMER_MAT0_OUT, &matchConfig0);
    CTIMER_SetupMatch(CTIMER, CTIMER_MAT1_OUT, &matchConfig1);
    CTIMER_StartTimer(CTIMER);

    while (1)
    {
    }
}
