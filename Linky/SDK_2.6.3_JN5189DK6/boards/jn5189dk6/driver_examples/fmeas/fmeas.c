/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_inputmux.h"
#include "board.h"
#include "fsl_fmeas.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
//#define EXAMPLE_REFERENCE_CLOCK (kINPUTMUX_ClkInToFreqmeas)
#define EXAMPLE_REFERENCE_CLOCK (kINPUTMUX_Xtal32MhzToFreqmeas)
#define EXAMPLE_TARGET_CLOCK_1 (kINPUTMUX_Fro1MhzToFreqmeas)
#define EXAMPLE_TARGET_CLOCK_2 (kINPUTMUX_32KhzOscToFreqmeas)
#define EXAMPLE_TARGET_CLOCK_3 (kINPUTMUX_32KhzOscToFreqmeas)
#define EXAMPLE_TARGET_CLOCK_4 (kINPUTMUX_ClkInToFreqmeas)

#define EXAMPLE_REFERENCE_CLOCK_NAME "Xtal 32Mhz"
#define EXAMPLE_TARGET_CLOCK_1_NAME "FRO 1MHz oscillator"
#define EXAMPLE_TARGET_CLOCK_2_NAME "XTAL 32K oscillator"
#define EXAMPLE_TARGET_CLOCK_3_NAME "FRO 32K oscillator"
#define EXAMPLE_TARGET_CLOCK_4_NAME "External clock in"

#define EXAMPLE_REFERENCE_CLOCK_ENABLE (kCLOCK_Xtal32M)
#define EXAMPLE_TARGET_CLOCK_1_ENABLE (kCLOCK_Fro1M)
#define EXAMPLE_TARGET_CLOCK_2_ENABLE (kCLOCK_Xtal32k)
#define EXAMPLE_TARGET_CLOCK_3_ENABLE (kCLOCK_Fro32k)
#define EXAMPLE_TARGET_CLOCK_4_ENABLE (kCLOCK_ExtClk)

#define EXAMPLE_REFERENCE_CLOCK_FREQUENCY (CLOCK_GetFreq(kCLOCK_Xtal32M))
#define EXAMPLE_TARGET_CLOCK_1_FREQUENCY (CLOCK_GetFreq(kCLOCK_Fro1M))
#define EXAMPLE_TARGET_CLOCK_2_FREQUENCY (CLOCK_GetFreq(kCLOCK_Xtal32k))
#define EXAMPLE_TARGET_CLOCK_3_FREQUENCY (CLOCK_GetFreq(kCLOCK_Fro32k))
#define EXAMPLE_TARGET_CLOCK_4_FREQUENCY (CLOCK_GetFreq(kCLOCK_ExtClk))

#define EXAMPLE_REFERENCE_CLOCK_REGISTRY_INDEX (0U)
#define EXAMPLE_TARGET_CLOCK_REGISTRY_INDEX (1U)

#define DEMO_USART USART0
#define DEMO_USART_CLK_SRC kCLOCK_Fro32M
#define DEMO_USART_IRQn USART0_IRQn
#define DEMO_USART_IRQHandler FLEXCOMM0_IRQHandler
#define USART_TX_DMA_CHANNEL 1
#define USART_RX_DMA_CHANNEL 0

#if defined(FSL_FEATURE_FMEAS_USE_ASYNC_SYSCON) && (FSL_FEATURE_FMEAS_USE_ASYNC_SYSCON)
#define FMEAS_SYSCON ASYNC_SYSCON
#else
#define FMEAS_SYSCON SYSCON
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief    Measurement cycle with value display
 *
 * @param    srcName : Name of the measured target clock
 * @param    target  : Target (measured) clock
 * @param    freqRef : Reference frequency [Hz]
 * @param    freqExp : Expected frequency [Hz]
 *
 * @return   Measured frequency [Hz]
 */
static uint32_t MeasureDisplay(char *srcName, inputmux_connection_t target, uint32_t freqRef, uint32_t freqExp)
{
    uint32_t freqComp;

    /* Setup to measure the selected target */
    INPUTMUX_AttachSignal(INPUTMUX, EXAMPLE_TARGET_CLOCK_REGISTRY_INDEX, target);

    /* Start a measurement cycle and wait for it to complete. If the target
       clock is not running, the measurement cycle will remain active
       forever, so a timeout may be necessary if the target clock can stop */
    FMEAS_StartMeasure(FMEAS_SYSCON);
    while (!FMEAS_IsMeasureComplete(FMEAS_SYSCON))
    {
    }

    /* Get computed frequency */
    freqComp = FMEAS_GetFrequency(FMEAS_SYSCON, freqRef);

    /* Show the raw capture value and the compute frequency */
    PRINTF("Capture source: %s, reference frequency = %u Hz\r\n", srcName, freqRef);
    PRINTF("Computed frequency value = %u Hz\r\n", freqComp);
    PRINTF("Expected frequency value = %u Hz\r\n\r\n", freqExp);

    return freqComp;
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    /* Enables the clock for the InputMux */
    CLOCK_EnableClock(kCLOCK_InputMux);

    /* Enable XTAL 32MHz (FMEAS reference clock) */
    CLOCK_EnableClock(kCLOCK_Xtal32M);

    /* Enable OSC32k on XTAL32k (steps 2) */
    CLOCK_EnableClock(kCLOCK_Xtal32k);
    CLOCK_uDelay(500000);

    /* Ungate 32k clock path to FMEAS (steps 2-3) */
    SYSCON->MODEMCTRL |= SYSCON_MODEMCTRL_BLE_LP_OSC32K_EN_MASK;

    /* Route FRO1M to CLKOUT (step 4) */
    CLOCK_AttachClk(kFRO1M_to_CLKOUT);

    uint32_t freqRef = EXAMPLE_REFERENCE_CLOCK_FREQUENCY;

    INPUTMUX_AttachSignal(INPUTMUX, EXAMPLE_REFERENCE_CLOCK_REGISTRY_INDEX, EXAMPLE_REFERENCE_CLOCK);

    /* Start frequency measurement 1 and display results */
    MeasureDisplay(EXAMPLE_TARGET_CLOCK_1_NAME " (" EXAMPLE_REFERENCE_CLOCK_NAME " reference)", EXAMPLE_TARGET_CLOCK_1,
                   freqRef, EXAMPLE_TARGET_CLOCK_1_FREQUENCY);

    /* Start frequency measurement 2 and display results */
    MeasureDisplay(EXAMPLE_TARGET_CLOCK_2_NAME " (" EXAMPLE_REFERENCE_CLOCK_NAME " reference)", EXAMPLE_TARGET_CLOCK_2,
                   freqRef, EXAMPLE_TARGET_CLOCK_2_FREQUENCY);

    /* Start frequency measurement 3 and display results */
    MeasureDisplay(EXAMPLE_TARGET_CLOCK_3_NAME " (" EXAMPLE_REFERENCE_CLOCK_NAME " reference)", EXAMPLE_TARGET_CLOCK_3,
                   freqRef, EXAMPLE_TARGET_CLOCK_3_FREQUENCY);

    while (1)
    {
    }
}
