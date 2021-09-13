/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_pint.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_clock.h"
#include "fsl_inputmux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_NUMBER_BUTTONS_ON_BOARD 2
#define DEMO_PINT_BSLICE0_SRC kPINT_PatternMatchInp0Src
#define DEMO_PINT_BSLICE1_SRC kPINT_PatternMatchInp0Src
#define DEMO_PINT_BSLICE2_SRC kPINT_PatternMatchInp1Src
#define DEMO_PINT_PIN_INT0_SRC kINPUTMUX_GpioPort0Pin4ToPintsel
#define DEMO_PINT_PIN_INT1_SRC kINPUTMUX_GpioPort0Pin19ToPintsel

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/


/*!
 * @brief Call back for PINT Pin interrupt 0-7.
 */
void pint_intr_callback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
    PRINTF("\f\r\nPINT Pin Interrupt %d event detected. PatternMatch status = %8b\r\n", pintr, pmatch_status);
}

/*!
 * @brief Main function
 */
int main(void)
{
    pint_pmatch_cfg_t pmcfg;

    /* Board pin, clock, debug console init */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* Init the boards */
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    CLOCK_uDelay(10000);

    /* Connect trigger sources to PINT */
    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, DEMO_PINT_PIN_INT0_SRC);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt1, DEMO_PINT_PIN_INT1_SRC);

    /* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX);

    /* Clear screen*/
    PRINTF("%c[2J", 27);
    /* Set cursor location at [0,0] */
    PRINTF("%c[0;0H", 27);
    PRINTF("\f\r\nPINT Pattern Match example\r\n");

    /* Initialize PINT */
    PINT_Init(PINT);

    /* configure kPINT_PatternMatchBSlice0 to show the single inputsrc */
    /* Setup Pattern Match Bit Slice 0 */
    pmcfg.bs_src    = DEMO_PINT_BSLICE0_SRC;
    pmcfg.bs_cfg    = kPINT_PatternMatchStickyFall;
    pmcfg.callback  = pint_intr_callback;
    pmcfg.end_point = true;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice0, &pmcfg);

    /* Enable callbacks for PINT0 by Index */
    PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);

    /* configure kPINT_PatternMatchBSlice1 and kPINT_PatternMatchBSlice2 to show the combined inputsrc */
#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 1U)
    /* Setup Pattern Match Bit Slice 1 */
    pmcfg.bs_src    = DEMO_PINT_BSLICE1_SRC;
    pmcfg.bs_cfg    = kPINT_PatternMatchStickyRise;
    pmcfg.callback  = pint_intr_callback;
    pmcfg.end_point = false;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice1, &pmcfg);
#endif

#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 2U)
    /* Setup Pattern Match Bit Slice 2 for falling edge detection */
    pmcfg.bs_src = DEMO_PINT_BSLICE2_SRC;
    pmcfg.bs_cfg = kPINT_PatternMatchStickyRise;
    pmcfg.callback = pint_intr_callback;
#if defined(DEMO_NUMBER_BUTTONS_ON_BOARD) && (DEMO_NUMBER_BUTTONS_ON_BOARD > 2)
    pmcfg.end_point = true;
#else
    pmcfg.end_point = false;
#endif
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice2, &pmcfg);

    /* Enable callbacks for PINT2 by Index */
    PINT_EnableCallbackByIndex(PINT, kPINT_PinInt2);
#endif

    /* Enable PatternMatch */
    PINT_PatternMatchEnable(PINT);

    PRINTF("\r\nPINT Pattern match events are configured\r\n");
    PRINTF("\r\nPress corresponding switches to generate events\r\n");
    while (1)
    {
        __WFI();
    }
}
