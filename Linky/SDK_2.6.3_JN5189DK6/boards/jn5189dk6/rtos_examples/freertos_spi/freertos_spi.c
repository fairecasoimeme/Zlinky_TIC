/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_spi.h"
#include "fsl_spi_freertos.h"
#include "board.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_SPI_SSEL kSPI_Ssel0
#define EXAMPLE_SLAVE_SPI_SPOL kSPI_SpolActiveAllLow
#define EXAMPLE_MASTER_SPI_SPOL kSPI_SpolActiveAllLow
#define EXAMPLE_SPI_MASTER SPI1
#define EXAMPLE_SPI_SLAVE SPI0
#define EXAMPLE_SPI_MASTER_IRQ SPI1_IRQn
#define EXAMPLE_SPI_SLAVE_IRQ SPI0_IRQn
#define TRANSFER_SIZE (32)          /*! Transfer size */
#define TRANSFER_BAUDRATE (500000U) /*! Transfer baudrate - 500k */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* SPI user callback */
void SPI_SlaveUserCallback(SPI_Type *base, spi_slave_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
spi_slave_handle_t g_s_handle;
uint8_t masterReceiveBuffer[TRANSFER_SIZE] = {0};
uint8_t masterSendBuffer[TRANSFER_SIZE]    = {0};
uint8_t slaveReceiveBuffer[TRANSFER_SIZE]  = {0};
uint8_t slaveSendBuffer[TRANSFER_SIZE]     = {0};

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
#define slave_task_PRIORITY (configMAX_PRIORITIES - 2)
#define master_task_PRIORITY (configMAX_PRIORITIES - 1)
#define SPI_NVIC_PRIO 2

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void slave_task(void *pvParameters);

static void master_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{
    int i;

    /* Init board hardware. */
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

    PRINTF("FreeRTOS SPI example start.\r\n");
    PRINTF("This example use one SPI instance as master and another as slave on one board.\r\n");
    PRINTF("Master and slave are both use interrupt way.\r\n");
    PRINTF("Please make sure you make the correct line connection. Basically, the connection is: \r\n");
    PRINTF(" SPI_master -- SPI_slave   \r\n");
    PRINTF("   SCK      --    SCK  \r\n");
    PRINTF("   PCS0     --    PCS0 \r\n");
    PRINTF("   SOUT     --    SIN  \r\n");
    PRINTF("   SIN      --    SOUT \r\n");

    /* Initialize data in transfer buffers */
    for (i = 0; i < TRANSFER_SIZE; i++)
    {
        masterSendBuffer[i]    = i % 256;
        masterReceiveBuffer[i] = 0;

        slaveSendBuffer[i]    = ~masterSendBuffer[i];
        slaveReceiveBuffer[i] = 0;
    }

    if (xTaskCreate(slave_task, "Slave_task", configMINIMAL_STACK_SIZE + 100, NULL, slave_task_PRIORITY, NULL) !=
        pdPASS)
    {
        PRINTF("Failed to create slave task");
        while (1)
            ;
    }

    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Data structure and callback function for slave SPI communication.
 */

typedef struct _callback_message_t
{
    status_t async_status;
    SemaphoreHandle_t sem;
} callback_message_t;

void SPI_SlaveUserCallback(SPI_Type *base, spi_slave_handle_t *handle, status_t status, void *userData)
{
    callback_message_t *cb_msg = (callback_message_t *)userData;
    BaseType_t reschedule      = 0;

    cb_msg->async_status = status;
    xSemaphoreGiveFromISR(cb_msg->sem, &reschedule);
    portYIELD_FROM_ISR(reschedule);
}
/*!
 * @brief Task responsible for slave SPI communication.
 */
static void slave_task(void *pvParameters)
{
    spi_slave_config_t slaveConfig;
    spi_transfer_t slaveXfer;
    uint32_t errorCount;
    uint32_t i;
    callback_message_t cb_msg;

    cb_msg.sem = xSemaphoreCreateBinary();
    if (cb_msg.sem == NULL)
    {
        PRINTF("SPI slave: Error creating semaphore\r\n");
        vTaskSuspend(NULL);
    }
    else
    {
        SPI_SlaveGetDefaultConfig(&slaveConfig);
        slaveConfig.sselPol = (spi_spol_t)EXAMPLE_SLAVE_SPI_SPOL;
        SPI_SlaveInit(EXAMPLE_SPI_SLAVE, &slaveConfig);
        SPI_SlaveTransferCreateHandle(EXAMPLE_SPI_SLAVE, &g_s_handle, SPI_SlaveUserCallback, &cb_msg);

        NVIC_SetPriority(EXAMPLE_SPI_SLAVE_IRQ, SPI_NVIC_PRIO);

        /*Set slave transfer ready to receive/send data*/
        slaveXfer.txData      = slaveSendBuffer;
        slaveXfer.dataSize    = TRANSFER_SIZE;
        slaveXfer.rxData      = slaveReceiveBuffer;
        slaveXfer.configFlags = 0;

        SPI_SlaveTransferNonBlocking(EXAMPLE_SPI_SLAVE, &g_s_handle, &slaveXfer);

        if (xTaskCreate(master_task, "Master_task", configMINIMAL_STACK_SIZE + 100, NULL, master_task_PRIORITY, NULL) !=
            pdPASS)
        {
            PRINTF("Failed to create master task");
            vTaskSuspend(NULL);
        }

        /* Wait for transfer to finish */
        xSemaphoreTake(cb_msg.sem, portMAX_DELAY);
        if (cb_msg.async_status == kStatus_SPI_Idle)
        {
            PRINTF("SPI slave transfer completed successfully. \r\n");
        }
        else
        {
            PRINTF("SPI slave transfer completed with error. \r\n");
        }

        errorCount = 0;
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            if (slaveSendBuffer[i] != masterReceiveBuffer[i])
            {
                errorCount++;
            }
        }
        if (errorCount == 0)
        {
            PRINTF("Master-to-slave data verified ok.\r\n");
        }
        else
        {
            PRINTF("Mismatch in master-to-slave data!\r\n");
        }

        vTaskSuspend(NULL);
    }
}
/*!
 * @brief Task responsible for master SPI communication.
 */
static void master_task(void *pvParameters)
{
    spi_master_config_t masterConfig;
    spi_rtos_handle_t master_rtos_handle;
    spi_transfer_t masterXfer = {0};
    uint32_t sourceClock;
    status_t status;

    uint32_t errorCount;
    uint32_t i;
    NVIC_SetPriority(EXAMPLE_SPI_MASTER_IRQ, SPI_NVIC_PRIO + 1);

    SPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = 500000;
    masterConfig.sselNum      = EXAMPLE_SPI_SSEL;
    masterConfig.sselPol      = (spi_spol_t)EXAMPLE_MASTER_SPI_SPOL;

    sourceClock = 12000000;
    status      = SPI_RTOS_Init(&master_rtos_handle, EXAMPLE_SPI_MASTER, &masterConfig, sourceClock);

    if (status != kStatus_Success)
    {
        PRINTF("SPI master: error during initialization. \r\n");
        vTaskSuspend(NULL);
    }

    /*Start master transfer*/
    masterXfer.txData   = masterSendBuffer;
    masterXfer.dataSize = TRANSFER_SIZE;
    masterXfer.rxData   = masterReceiveBuffer;
    masterXfer.configFlags |= kSPI_FrameAssert;

    status = SPI_RTOS_Transfer(&master_rtos_handle, &masterXfer);

    PRINTF("Master transmited:");
    for (i = 0; i < TRANSFER_SIZE; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", masterSendBuffer[i]);
    }
    PRINTF("\r\n\r\n");

    PRINTF("Slave received:");
    for (i = 0; i < TRANSFER_SIZE; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", slaveReceiveBuffer[i]);
    }
    PRINTF("\r\n\r\n");

    PRINTF("Slave transmited:");
    for (i = 0; i < TRANSFER_SIZE; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", slaveSendBuffer[i]);
    }
    PRINTF("\r\n\r\n");

    PRINTF("Master received:");
    for (i = 0; i < TRANSFER_SIZE; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", masterReceiveBuffer[i]);
    }
    PRINTF("\r\n\r\n");

    if (status == kStatus_Success)
    {
        PRINTF("SPI master transfer completed successfully.\r\n");
    }
    else
    {
        PRINTF("SPI master transfer completed with error.\r\n");
    }

    errorCount = 0;

    for (i = 0; i < TRANSFER_SIZE; i++)
    {
        if (masterReceiveBuffer[i] != slaveSendBuffer[i])
        {
            errorCount++;
        }
    }

    if (errorCount == 0)
    {
        PRINTF("Slave-to-master data verified ok.\r\n");
    }
    else
    {
        PRINTF("Mismatch in slave-to-master data!\r\n");
    }

    vTaskSuspend(NULL);
}
