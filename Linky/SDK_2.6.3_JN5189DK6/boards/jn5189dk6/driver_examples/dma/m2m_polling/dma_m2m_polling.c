/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_dma.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_DMA_CHANNEL 0
#define BUFF_LENGTH 4U
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint32_t s_srcBuffer[BUFF_LENGTH]  = {1, 2, 3, 4};
static uint32_t s_destBuffer[BUFF_LENGTH] = {0x00};
static volatile uint32_t s_transferCount  = 0;
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t i       = 0;
    uint32_t xferCfg = DMA_CHANNEL_XFER(true, false, false, true, 4U, kDMA_AddressInterleave1xWidth,
                                        kDMA_AddressInterleave1xWidth, 16U);
    uint32_t desc[sizeof(dma_descriptor_t)] = {0};

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

    /* Print source buffer */
    PRINTF("DMA memory to memory polling example begin.\r\n");
    PRINTF("Destination Buffer:\r\n");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", s_destBuffer[i]);
    }

    DMA_Init(DMA0);
    DMA_EnableChannel(DMA0, DEMO_DMA_CHANNEL);
    DMA_SetupDescriptor((dma_descriptor_t *)desc, xferCfg, s_srcBuffer, &s_destBuffer[0], NULL);
    DMA_LoadChannelDescriptor(DMA0, DEMO_DMA_CHANNEL, (dma_descriptor_t *)desc);
    DMA_DoChannelSoftwareTrigger(DMA0, DEMO_DMA_CHANNEL);
    /* Wait for DMA transfer finish */
    while (DMA_ChannelIsBusy(DMA0, DEMO_DMA_CHANNEL))
    {
    }
    /* Print destination buffer */
    PRINTF("\r\nDMA memory to memory polling example finish.\r\n");
    PRINTF("Destination Buffer:\r\n");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", s_destBuffer[i]);
    }
    while (1)
    {
    }
}
