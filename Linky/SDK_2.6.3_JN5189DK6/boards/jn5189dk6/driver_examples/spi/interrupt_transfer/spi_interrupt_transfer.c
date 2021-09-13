/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_spi.h"

#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_USART USART0
#define DEMO_USART_CLK_SRC kCLOCK_Fro32M
#define EXAMPLE_SPI_MASTER_CLK_SRC kCLOCK_Fro32M
#define EXAMPLE_SPI_MASTER_CLK_FREQ CLOCK_GetFreq(EXAMPLE_SPI_MASTER_CLK_SRC)
#define EXAMPLE_SPI_SSEL 0
#define EXAMPLE_SPI_MASTER SPI1
#define EXAMPLE_SPI_SLAVE SPI0
#define EXAMPLE_SPI_MASTER_IRQ SPI1_IRQn
#define EXAMPLE_SPI_SLAVE_IRQ SPI0_IRQn
#define MASTER_BAUDRATE 150000L
#define EXAMPLE_MASTER_SPI_SPOL kSPI_SpolActiveAllLow
#define EXAMPLE_SLAVE_SPI_SPOL kSPI_SpolActiveAllLow


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
#define BUFFER_SIZE (64)
static uint8_t srcBuff[BUFFER_SIZE];
static uint8_t destBuff[BUFFER_SIZE];
static spi_master_handle_t masterHandle;
static spi_slave_handle_t slaveHandle;
static volatile bool masterFinished = false;
static volatile bool slaveFinished  = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
static void masterCallback(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData)
{
    masterFinished = true;
}

static void slaveCallback(SPI_Type *base, spi_slave_handle_t *handle, status_t status, void *userData)
{
    slaveFinished = true;
}

int main(void)
{
    spi_master_config_t masterConfig = {0};
    spi_slave_config_t slaveConfig   = {0};
    spi_transfer_t xfer              = {0};
    uint32_t sourceClock             = 0U;
    uint32_t i                       = 0U;
    uint32_t err                     = 0U;

    /* Init the boards */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    PRINTF("\r\nSPI one board interrupt example started!\r\n");

    /* Init SPI master */
    /*
     * masterConfig.enableLoopback = false;
     * masterConfig.polarity = flexSPI_ClockPolarityActiveHigh;
     * masterConfig.phase = flexSPI_ClockPhaseFirstEdge;
     * masterConfig.direction = flexSPI_MsbFirst;
     * masterConfig.baudRate_Bps = 500000U;
     * masterConfig.dataWidth = flexSPI_Data8Bits;
     * config->sselNum = flexSPI_Ssel0;
     */
    SPI_MasterGetDefaultConfig(&masterConfig);
    sourceClock          = EXAMPLE_SPI_MASTER_CLK_FREQ;
    masterConfig.sselNum = (spi_ssel_t)EXAMPLE_SPI_SSEL;
    masterConfig.sselPol = (spi_spol_t)EXAMPLE_MASTER_SPI_SPOL;
    SPI_MasterInit(EXAMPLE_SPI_MASTER, &masterConfig, sourceClock);

    /* Create handle for SPI master */
    SPI_MasterTransferCreateHandle(EXAMPLE_SPI_MASTER, &masterHandle, masterCallback, NULL);
    /* Init SPI slave */
    /*
     * slaveConfig.polarity = flexSPI_ClockPolarityActiveHigh;
     * slaveConfig.phase = flexSPI_ClockPhaseFirstEdge;
     * slaveConfig.direction = flexSPI_MsbFirst;
     * slaveConfig.dataWidth = flexSPI_Data8Bits;
     */
    SPI_SlaveGetDefaultConfig(&slaveConfig);
    slaveConfig.sselPol = (spi_spol_t)EXAMPLE_SLAVE_SPI_SPOL;
    SPI_SlaveInit(EXAMPLE_SPI_SLAVE, &slaveConfig);
    SPI_SlaveTransferCreateHandle(EXAMPLE_SPI_SLAVE, &slaveHandle, slaveCallback, NULL);

    /* Set priority, slave have higher priority */
    NVIC_SetPriority(EXAMPLE_SPI_MASTER_IRQ, 1U);
    NVIC_SetPriority(EXAMPLE_SPI_SLAVE_IRQ, 0U);

    /* Init source buffer */
    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        srcBuff[i] = i;
    }

    /* SPI slave transfer */
    xfer.rxData   = destBuff;
    xfer.dataSize = sizeof(destBuff);
    xfer.txData   = NULL;
    SPI_SlaveTransferNonBlocking(EXAMPLE_SPI_SLAVE, &slaveHandle, &xfer);

    /* SPI master start transfer */
    xfer.txData   = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData   = NULL;
    xfer.configFlags |= kSPI_FrameAssert;
    SPI_MasterTransferNonBlocking(EXAMPLE_SPI_MASTER, &masterHandle, &xfer);

    while ((masterFinished != true) || (slaveFinished != true))
    {
    }

    /* Check the data received */
    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        if (destBuff[i] != srcBuff[i])
        {
            PRINTF("\r\nThe %d data is wrong, the data received is %d \r\n", i, destBuff[i]);
            err++;
        }
    }
    if (err == 0U)
    {
        PRINTF("\r\nSPI transfer finished!\r\n");
    }

    while (1)
    {
    }
}
