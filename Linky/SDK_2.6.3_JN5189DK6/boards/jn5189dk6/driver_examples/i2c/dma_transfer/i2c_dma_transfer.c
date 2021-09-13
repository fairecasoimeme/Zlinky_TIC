/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*  SDK Included Files */
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_i2c_dma.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_USART USART0
#define DEMO_USART_CLK_SRC kCLOCK_Fro32M
#define I2C_MASTER_CLOCK_FREQUENCY CLOCK_GetFreq(kCLOCK_Fro32M)
#define I2C_SLAVE_CLOCK_FREQUENCY CLOCK_GetFreq(kCLOCK_Fro32M)
#define EXAMPLE_I2C_MASTER_BASE I2C0_BASE
#define EXAMPLE_I2C_SLAVE_BASE I2C1_BASE

#define EXAMPLE_DMA DMA0
#define EXAMPLE_I2C_MASTER_CHANNEL 5

#define EXAMPLE_I2C_MASTER ((I2C_Type *)EXAMPLE_I2C_MASTER_BASE)
#define EXAMPLE_I2C_SLAVE ((I2C_Type *)EXAMPLE_I2C_SLAVE_BASE)

#define I2C_MASTER_SLAVE_ADDR_7BIT (0x7EU)
#define I2C_BAUDRATE (100000) /* 100K */
#define I2C_DATA_LENGTH (32)  /* MAX is 256 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_slave_buff[I2C_DATA_LENGTH];
uint8_t g_master_buff[I2C_DATA_LENGTH];

i2c_master_dma_handle_t g_m_dma_handle;
i2c_slave_handle_t g_s_handle;
dma_handle_t dmaHandle;
volatile bool g_slaveCompleted  = false;
volatile bool g_masterCompleted = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void i2c_slave_callback(I2C_Type *base, volatile i2c_slave_transfer_t *xfer, void *param)
{
    switch (xfer->event)
    {
        /* Setup the slave receive buffer */
        case kI2C_SlaveReceiveEvent:
            xfer->rxData = g_slave_buff;
            xfer->rxSize = I2C_DATA_LENGTH;
            break;

        /* The master has sent a stop transition on the bus */
        case kI2C_SlaveCompletionEvent:
            g_slaveCompleted = true;
            break;

        /* Transmit event not handled in this demo */
        case kI2C_SlaveTransmitEvent:
            xfer->txData = g_slave_buff;
            xfer->txSize = I2C_DATA_LENGTH;
            break;

        default:
            break;
    }
}

static void i2c_master_dma_callback(I2C_Type *base, i2c_master_dma_handle_t *handle, status_t status, void *userData)
{
    g_masterCompleted = true;
}

int main(void)
{
    i2c_slave_config_t slaveConfig;

    i2c_master_config_t masterConfig;
    i2c_master_transfer_t masterXfer;

    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    DMA_Init(EXAMPLE_DMA);

    PRINTF("\r\nI2C example -- MasterDMA_SlaveInterrupt.\r\n");

    /*1.Set up i2c slave first*/
    /*
     * slaveConfig.addressingMode = kI2C_Address7bit;
     * slaveConfig.enableGeneralCall = false;
     * slaveConfig.enableWakeUp = false;
     * slaveConfig.enableBaudRateCtl = false;
     * slaveConfig.enableSlave = true;
     */
    I2C_SlaveGetDefaultConfig(&slaveConfig);

    /* Change the slave address */
    slaveConfig.address0.address = I2C_MASTER_SLAVE_ADDR_7BIT;

    /* Initialize the I2C slave peripheral */
    I2C_SlaveInit(EXAMPLE_I2C_SLAVE, &slaveConfig, I2C_SLAVE_CLOCK_FREQUENCY);

    memset(g_slave_buff, 0, sizeof(g_slave_buff));

    I2C_SlaveTransferCreateHandle(EXAMPLE_I2C_SLAVE, &g_s_handle, i2c_slave_callback, NULL);
    I2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle, kI2C_SlaveCompletionEvent);
    /*2.Set up i2c master to send data to slave*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        g_master_buff[i] = i;
    }

    PRINTF("Master will send data :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0U)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_master_buff[i]);
    }
    PRINTF("\r\n\r\n");

    /*
     * masterConfig.baudRate_Bps = 100000U;
     * masterConfig.enableStopHold = false;
     * masterConfig.glitchFilterWidth = 0U;
     * masterConfig.enableMaster = true;
     */
    I2C_MasterGetDefaultConfig(&masterConfig);

    /* Change the default baudrate configuration */
    masterConfig.baudRate_Bps = I2C_BAUDRATE;

    /* Initialize the I2C master peripheral */
    I2C_MasterInit(EXAMPLE_I2C_MASTER, &masterConfig, I2C_MASTER_CLOCK_FREQUENCY);

    memset(&g_m_dma_handle, 0, sizeof(g_m_dma_handle));
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT;
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data           = g_master_buff;
    masterXfer.dataSize       = I2C_DATA_LENGTH;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    DMA_EnableChannel(EXAMPLE_DMA, EXAMPLE_I2C_MASTER_CHANNEL);
    DMA_CreateHandle(&dmaHandle, EXAMPLE_DMA, EXAMPLE_I2C_MASTER_CHANNEL);

    I2C_MasterTransferCreateHandleDMA(EXAMPLE_I2C_MASTER, &g_m_dma_handle, i2c_master_dma_callback, NULL, &dmaHandle);
    I2C_MasterTransferDMA(EXAMPLE_I2C_MASTER, &g_m_dma_handle, &masterXfer);

    /*  wait for transfer completed. */
    while (!g_slaveCompleted)
    {
    }
    g_slaveCompleted = false;

    /*3.Transfer completed. Check the data.*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (g_slave_buff[i] != g_master_buff[i])
        {
            PRINTF("\r\nError occurred in this transfer ! \r\n");
            break;
        }
    }

    PRINTF("Slave received data :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_slave_buff[i]);
    }
    PRINTF("\r\n\r\n");

    /*4.Set up slave ready to send data to master.*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        g_slave_buff[i] = ~g_slave_buff[i];
    }

    PRINTF("This time , slave will send data: :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_slave_buff[i]);
    }
    PRINTF("\r\n\r\n");

    /*  Already setup the slave transfer in item 1 */

    /*5.Set up master to receive data from slave.*/

    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        g_master_buff[i] = 0;
    }

    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data           = g_master_buff;
    masterXfer.dataSize       = I2C_DATA_LENGTH;

    masterXfer.flags = kI2C_TransferDefaultFlag;

    g_masterCompleted = false;
    I2C_MasterTransferDMA(EXAMPLE_I2C_MASTER, &g_m_dma_handle, &masterXfer);

    /*  wait for transfer completed. */
    while (!g_masterCompleted)
    {
    }

    /*6.Transfer completed. Check the data.*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (g_slave_buff[i] != g_master_buff[i])
        {
            PRINTF("\r\nError occurred in the transfer ! \r\n");
            break;
        }
    }

    PRINTF("Master received data :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_master_buff[i]);
    }
    PRINTF("\r\n\r\n");

    PRINTF("\r\nEnd of I2C example .\r\n");
    while (1)
    {
    }
}
