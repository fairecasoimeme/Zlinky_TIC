/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "fsl_spi.h"
#include "fsl_spi_dma.h"
#include "fsl_dma.h"
#include "board.h"
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
#define EXAMPLE_SPI_SLAVE SPI0
#define EXAMPLE_SPI_MASTER_IRQ SPI1_IRQn
#define EXAMPLE_SPI_SLAVE_IRQ SPI0_IRQn

#define EXAMPLE_DMA DMA0

#define EXAMPLE_SPI_MASTER_RX_CHANNEL 10
#define EXAMPLE_SPI_MASTER_TX_CHANNEL 11

#define EXAMPLE_SPI_SLAVE_RX_CHANNEL 8
#define EXAMPLE_SPI_SLAVE_TX_CHANNEL 9

#define EXAMPLE_MASTER_SPI_SPOL kSPI_SpolActiveAllLow
#define EXAMPLE_SLAVE_SPI_SPOL kSPI_SpolActiveAllLow

#define MASTER_BAUDRATE 300000L

#define TRANSFER_SIZE 256U /*! Transfer dataSize */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* SPI user callback */
void SPI_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData);
void SPI_SlaveUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t masterRxData[TRANSFER_SIZE] = {0};
uint8_t masterTxData[TRANSFER_SIZE] = {0};
uint8_t slaveRxData[TRANSFER_SIZE]  = {0};
uint8_t slaveTxData[TRANSFER_SIZE]  = {0};

dma_handle_t masterTxHandle;
dma_handle_t masterRxHandle;
dma_handle_t slaveTxHandle;
dma_handle_t slaveRxHandle;

spi_dma_handle_t masterHandle;
spi_dma_handle_t slaveHandle;

volatile bool isTransferCompleted = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

void SPI_SlaveUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        PRINTF("This is SPI slave dma transfer completed callback. \r\n\r\n");
    }
    isTransferCompleted = true;
}

void SPI_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        PRINTF("This is SPI master dma transfer completed callback. \r\n\r\n");
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    PRINTF("SPI dma example start.\r\n");
    PRINTF("This example use one spi instance as master and another as slave on one board.\r\n");
    PRINTF("Please make sure you make the correct line connection. Basically, the connection is: \r\n");
    PRINTF("SPI_master -- SPI_slave   \r\n");
    PRINTF("   CLK      --    CLK  \r\n");
    PRINTF("   PCS      --    PCS  \r\n");
    PRINTF("   SOUT     --    SIN  \r\n");
    PRINTF("   SIN      --    SOUT \r\n");

    /* DMA init */
    DMA_Init(EXAMPLE_DMA);

    /* SPI init */
    uint32_t srcClock_Hz;
    uint32_t errorCount;
    uint32_t i;
    spi_master_config_t masterConfig;
    spi_slave_config_t slaveConfig;
    spi_transfer_t masterXfer = {0};
    spi_transfer_t slaveXfer  = {0};

    srcClock_Hz = EXAMPLE_SPI_MASTER_CLK_FREQ;

    /* Master config */
    SPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.sselNum = (spi_ssel_t)EXAMPLE_SPI_SSEL;
    masterConfig.sselPol = (spi_spol_t)EXAMPLE_MASTER_SPI_SPOL;
    SPI_MasterInit(EXAMPLE_SPI_MASTER, &masterConfig, srcClock_Hz);

    /* Slave config */
    SPI_SlaveGetDefaultConfig(&slaveConfig);
    /* SPI slave must be enabled afer DMA is ready to transfer data */
    slaveConfig.enableSlave = false;
    slaveConfig.sselPol     = (spi_spol_t)EXAMPLE_SLAVE_SPI_SPOL;
    SPI_SlaveInit(EXAMPLE_SPI_SLAVE, &slaveConfig);

    /* Set up the transfer data */
    for (i = 0U; i < TRANSFER_SIZE; i++)
    {
        /* SPI is configured for 8 bits transfer - set only lower 8 bits of buffers */
        masterTxData[i] = (uint8_t)(i % 256U);
        masterRxData[i] = (uint8_t)0U;

        slaveTxData[i] = (uint8_t)(~masterTxData[i]);
        slaveRxData[i] = (uint8_t)0U;
    }

    DMA_EnableChannel(EXAMPLE_DMA, EXAMPLE_SPI_MASTER_TX_CHANNEL);
    DMA_EnableChannel(EXAMPLE_DMA, EXAMPLE_SPI_MASTER_RX_CHANNEL);
    DMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_SPI_MASTER_TX_CHANNEL, kDMA_ChannelPriority3);
    DMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_SPI_MASTER_RX_CHANNEL, kDMA_ChannelPriority2);
    DMA_CreateHandle(&masterTxHandle, EXAMPLE_DMA, EXAMPLE_SPI_MASTER_TX_CHANNEL);
    DMA_CreateHandle(&masterRxHandle, EXAMPLE_DMA, EXAMPLE_SPI_MASTER_RX_CHANNEL);

    DMA_EnableChannel(EXAMPLE_DMA, EXAMPLE_SPI_SLAVE_TX_CHANNEL);
    DMA_EnableChannel(EXAMPLE_DMA, EXAMPLE_SPI_SLAVE_RX_CHANNEL);
    DMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_SPI_SLAVE_TX_CHANNEL, kDMA_ChannelPriority0);
    DMA_SetChannelPriority(EXAMPLE_DMA, EXAMPLE_SPI_SLAVE_RX_CHANNEL, kDMA_ChannelPriority1);
    DMA_CreateHandle(&slaveTxHandle, EXAMPLE_DMA, EXAMPLE_SPI_SLAVE_TX_CHANNEL);
    DMA_CreateHandle(&slaveRxHandle, EXAMPLE_DMA, EXAMPLE_SPI_SLAVE_RX_CHANNEL);

    isTransferCompleted = false;

    /* Set up spi slave first */
    SPI_SlaveTransferCreateHandleDMA(EXAMPLE_SPI_SLAVE, &slaveHandle, SPI_SlaveUserCallback, NULL, &slaveTxHandle,
                                     &slaveRxHandle);

    slaveXfer.txData   = (uint8_t *)&slaveTxData;
    slaveXfer.rxData   = (uint8_t *)&slaveRxData;
    slaveXfer.dataSize = TRANSFER_SIZE;

    if (kStatus_Success != SPI_SlaveTransferDMA(EXAMPLE_SPI_SLAVE, &slaveHandle, &slaveXfer))
    {
        PRINTF("There is error when start SPI_SlaveTransferDMA \r\n");
    }

    /* Enable SPI Slave after DMA is ready to transfer data */
    SPI_Enable(EXAMPLE_SPI_SLAVE, true);

    /* Set up spi master */
    SPI_MasterTransferCreateHandleDMA(EXAMPLE_SPI_MASTER, &masterHandle, SPI_MasterUserCallback, NULL, &masterTxHandle,
                                      &masterRxHandle);

    /* Start master transfer */
    masterXfer.txData   = (uint8_t *)&masterTxData;
    masterXfer.rxData   = (uint8_t *)&masterRxData;
    masterXfer.dataSize = TRANSFER_SIZE;
    masterXfer.configFlags |= kSPI_FrameAssert;

    if (kStatus_Success != SPI_MasterTransferDMA(EXAMPLE_SPI_MASTER, &masterHandle, &masterXfer))
    {
        PRINTF("There is error when start SPI_MasterTransferDMA \r\n ");
    }

    /* Wait until transfer completed */
    while (!isTransferCompleted)
    {
    }

    /* Check the data */
    errorCount = 0U;
    for (i = 0U; i < TRANSFER_SIZE; i++)
    {
        if ((uint8_t)masterTxData[i] != (uint8_t)slaveRxData[i])
        {
            errorCount++;
        }
        if ((uint8_t)slaveTxData[i] != (uint8_t)masterRxData[i])
        {
            errorCount++;
        }
    }
    if (errorCount == 0)
    {
        PRINTF("SPI transfer all data matched! \r\n");
    }
    else
    {
        PRINTF("Error occurred in SPI transfer ! \r\n");
    }

    SPI_Deinit(EXAMPLE_SPI_MASTER);
    SPI_Deinit(EXAMPLE_SPI_SLAVE);

    while (1)
    {
    }
}
