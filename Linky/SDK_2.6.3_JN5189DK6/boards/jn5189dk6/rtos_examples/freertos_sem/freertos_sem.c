/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TASK_PRIO (configMAX_PRIORITIES - 1)
#define CONSUMER_LINE_SIZE 3
SemaphoreHandle_t xSemaphore_producer;
SemaphoreHandle_t xSemaphore_consumer;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void producer_task(void *pvParameters);
static void consumer_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
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
    RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
    if (xTaskCreate(producer_task, "PRODUCER_TASK", configMINIMAL_STACK_SIZE + 128, NULL, TASK_PRIO, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    /* Start scheduling. */
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Task producer_task.
 */
static void producer_task(void *pvParameters)
{
    uint32_t i;

    PRINTF("Producer_task created.\r\n");
    xSemaphore_producer = xSemaphoreCreateBinary();
    if (xSemaphore_producer == NULL)
    {
        PRINTF("xSemaphore_producer creation failed.\r\n");
        vTaskSuspend(NULL);
    }

    xSemaphore_consumer = xSemaphoreCreateBinary();
    if (xSemaphore_consumer == NULL)
    {
        PRINTF("xSemaphore_consumer creation failed.\r\n");
        vTaskSuspend(NULL);
    }

    for (i = 0; i < CONSUMER_LINE_SIZE; i++)
    {
        if (xTaskCreate(consumer_task, "CONSUMER_TASK", configMINIMAL_STACK_SIZE, (void *)i, TASK_PRIO, NULL) != pdPASS)
        {
            PRINTF("Task creation failed!.\r\n");
            vTaskSuspend(NULL);
        }
        else
        {
            PRINTF("Consumer_task %d created.\r\n", i);
        }
    }

    while (1)
    {
        /* Producer is ready to provide item. */
        xSemaphoreGive(xSemaphore_consumer);
        /* Producer is waiting when consumer will be ready to accept item. */
        if (xSemaphoreTake(xSemaphore_producer, portMAX_DELAY) == pdTRUE)
        {
            PRINTF("Producer released item.\r\n");
        }
        else
        {
            PRINTF("Producer is waiting for customer.\r\n");
        }
    }
}

/*!
 * @brief Task consumer_task.
 */
static void consumer_task(void *pvParameters)
{
    PRINTF("Consumer number: %d\r\n", pvParameters);
    while (1)
    {
        /* Consumer is ready to accept. */
        xSemaphoreGive(xSemaphore_producer);
        /* Consumer is waiting when producer will be ready to produce item. */
        if (xSemaphoreTake(xSemaphore_consumer, portMAX_DELAY) == pdTRUE)
        {
            PRINTF("Consumer %d accepted item.\r\n", pvParameters);
        }
        else
        {
            PRINTF("Consumer %d is waiting for producer.\r\n", pvParameters);
        }
    }
}
