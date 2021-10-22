/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_usart.h"
#include "fsl_cic_irb.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEBUG_USART USART0

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void msDelay(unsigned int milliSeconds);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static cic_irb_config_t config;

/*******************************************************************************
 * Code
 ******************************************************************************/

void msDelay(unsigned int milliSeconds)
{
    unsigned int i;
    volatile uint32_t x = 0;

    for (i = 0; i < milliSeconds * 1100; i++)
    {
        x = 1 - x;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    bool isBusy = false;
    uint32_t i  = 0;

    /* Init the boards */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
    PRINTF("IR Blaster example\r\n");
    PRINTF("Sending 10 RC-5 packets...\r\n");

    /* Get default configuration values */
    CIC_IRB_GetDefaultConfig(&config);

    /* Initialize and enable the IR peripheral */
    CIC_IRB_Init(CIC_IRB, &config);
    CIC_IRB_Enable(CIC_IRB);

    /* Send 10 packets */
    while (i < 10)
    {
        /* Send a fixed RC-5 packet */
        CIC_IRB_SendRC5Packet(CIC_IRB, true, 0x15, 0x38);

        /* Wait until message sending is complete */
        do
        {
            CIC_IRB_IsBusy(CIC_IRB, &isBusy);
        } while (!isBusy);

        PRINTF("Packet %d\r\n", ++i);

        /* Delay 500 ms */
        msDelay(500);
    }

    CIC_IRB_DeInit(CIC_IRB);

    PRINTF("Bye.\r\n");

    return 0;
}
