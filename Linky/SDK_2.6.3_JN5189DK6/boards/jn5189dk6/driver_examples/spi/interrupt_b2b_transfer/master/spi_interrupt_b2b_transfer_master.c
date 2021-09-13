/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_spi.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_USART USART0
#define DEMO_USART_CLK_SRC kCLOCK_Fro32M
#define EXAMPLE_SPI_MASTER_CLK_SRC kCLOCK_Fro32M
#define EXAMPLE_SPI_MASTER_CLK_FREQ CLOCK_GetFreq(EXAMPLE_SPI_MASTER_CLK_SRC)
#define EXAMPLE_SPI_SSEL 0
#define EXAMPLE_SPI_MASTER SPI1
#define EXAMPLE_SPI_MASTER_IRQ SPI1_IRQn
#define EXAMPLE_SPI_SPOL kSPI_SpolActiveAllLow


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
#define BUFFER_SIZE (64)
static uint8_t srcBuff[BUFFER_SIZE];
static uint8_t destBuff[BUFFER_SIZE];
spi_master_handle_t handle;
static volatile bool masterFinished = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
static void masterCallback(SPI_Type *base, spi_master_handle_t *masterHandle, status_t status, void *userData)
{
    masterFinished = true;
}

int main(void)
{
    spi_transfer_t xfer = {0};
    spi_master_config_t userConfig;
    uint32_t err     = 0;
    uint32_t i       = 0;
    uint32_t srcFreq = 0;

    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
    PRINTF("\n\rMaster Start...\n\r");
    /*
     * userConfig.enableLoopback = false;
     * userConfig.enableMaster = true;
     * userConfig.polarity = kSPI_ClockPolarityActiveHigh;
     * userConfig.phase = kSPI_ClockPhaseFirstEdge;
     * userConfig.direction = kSPI_MsbFirst;
     * userConfig.baudRate_Bps = 500000U;
     */
    SPI_MasterGetDefaultConfig(&userConfig);
    srcFreq            = EXAMPLE_SPI_MASTER_CLK_FREQ;
    userConfig.sselNum = (spi_ssel_t)EXAMPLE_SPI_SSEL;
    userConfig.sselPol = (spi_spol_t)EXAMPLE_SPI_SPOL;
    SPI_MasterInit(EXAMPLE_SPI_MASTER, &userConfig, srcFreq);

    /* Init Buffer */
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        srcBuff[i] = i;
    }

    /* Send to slave */
    xfer.txData   = srcBuff;
    xfer.rxData   = destBuff;
    xfer.dataSize = sizeof(destBuff);
    SPI_MasterTransferCreateHandle(EXAMPLE_SPI_MASTER, &handle, masterCallback, NULL);
    SPI_MasterTransferNonBlocking(EXAMPLE_SPI_MASTER, &handle, &xfer);

    while (masterFinished != true)
    {
    }

    /* Check if the data is right */
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        if (srcBuff[i] != destBuff[i])
        {
            err++;
            PRINTF("The %d is wrong! data is %d\n\r", i, destBuff[i]);
        }
    }
    if (err == 0)
    {
        PRINTF("Succeed!\n\r");
    }

    while (1)
    {
    }
}
