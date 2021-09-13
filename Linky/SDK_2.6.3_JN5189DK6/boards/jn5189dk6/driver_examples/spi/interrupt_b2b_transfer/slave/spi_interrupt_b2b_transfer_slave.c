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
#define EXAMPLE_SPI_SSEL 0
#define EXAMPLE_SPI_SLAVE SPI1
#define EXAMPLE_SPI_SLAVE_IRQ SPI1_IRQn
#define EXAMPLE_SPI_SPOL kSPI_SpolActiveAllLow


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
#define BUFFER_SIZE (64)
spi_slave_handle_t handle;
static uint8_t buff[BUFFER_SIZE];
static uint8_t sendBuff[BUFFER_SIZE];
static volatile bool slaveFinished = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
static void slaveCallback(SPI_Type *base, spi_slave_handle_t *slaveHandle, status_t status, void *userData)
{
    slaveFinished = true;
}

int main(void)
{
    uint32_t i          = 0;
    uint8_t err         = 0;
    spi_transfer_t xfer = {0};
    spi_slave_config_t userConfig;

    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
    PRINTF("\n\rSlave is working....\n\r");

    /*
     * userConfig.enableSlave = true;
     * userConfig.polarity = kSPI_ClockPolarityActiveHigh;
     * userConfig.phase = kSPI_ClockPhaseFirstEdge;
     * userConfig.direction = kSPI_MsbFirst;
     */
    SPI_SlaveGetDefaultConfig(&userConfig);
    userConfig.sselPol = (spi_spol_t)EXAMPLE_SPI_SPOL;
    SPI_SlaveInit(EXAMPLE_SPI_SLAVE, &userConfig);
    SPI_SlaveTransferCreateHandle(EXAMPLE_SPI_SLAVE, &handle, slaveCallback, NULL);
    for (i = 0; i < 64; i++)
    {
        sendBuff[i] = i;
    }

    /* receive data from master*/
    xfer.txData   = sendBuff;
    xfer.rxData   = buff;
    xfer.dataSize = sizeof(sendBuff);
    SPI_SlaveTransferNonBlocking(EXAMPLE_SPI_SLAVE, &handle, &xfer);

    while (slaveFinished != true)
    {
    }

    for (i = 0; i < BUFFER_SIZE; i++)
    {
        if (buff[i] != i)
        {
            PRINTF("\n\rThe %d number is wrong! It is %d\n\r", i, buff[i]);
            err++;
        }
    }
    PRINTF("\r\n");
    if (err == 0)
    {
        PRINTF("Succeed!\n\r");
    }

    while (1)
    {
    }
}
