/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*System includes.*/
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_LOG_LENGTH 20
/*******************************************************************************
 * Globals
 ******************************************************************************/
/* Logger queue handle */
static QueueHandle_t log_queue = NULL;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Application API */
static void write_task_1(void *pvParameters);
static void write_task_2(void *pvParameters);

/* Logger API */
void log_add(char *log);
void log_init(uint32_t queue_length, uint32_t max_log_lenght);
static void log_task(void *pvParameters);
/*******************************************************************************
 * Code
 ******************************************************************************/

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
    RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
    /* Initialize logger for 10 logs with maximum lenght of one log 20 B */
    log_init(10, MAX_LOG_LENGTH);
    if (xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 166, NULL, tskIDLE_PRIORITY + 2, NULL) !=
        pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(write_task_2, "WRITE_TASK_2", configMINIMAL_STACK_SIZE + 166, NULL, tskIDLE_PRIORITY + 2, NULL) !=
        pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    vTaskStartScheduler();
    for (;;)
        ;
}

/*******************************************************************************
 * Application functions
 ******************************************************************************/
/*!
 * @brief write_task_1 function
 */
static void write_task_1(void *pvParameters)
{
    char log[MAX_LOG_LENGTH + 1];
    uint32_t i = 0;
    for (i = 0; i < 5; i++)
    {
        sprintf(log, "Task1 Message %d", (int)i);
        log_add(log);
        taskYIELD();
    }
    vTaskSuspend(NULL);
}

/*!
 * @brief write_task_2 function
 */
static void write_task_2(void *pvParameters)
{
    char log[MAX_LOG_LENGTH + 1];
    uint32_t i = 0;
    for (i = 0; i < 5; i++)
    {
        sprintf(log, "Task2 Message %d", (int)i);
        log_add(log);
        taskYIELD();
    }
    vTaskSuspend(NULL);
}

/*******************************************************************************
 * Logger functions
 ******************************************************************************/
/*!
 * @brief log_add function
 */
void log_add(char *log)
{
    xQueueSend(log_queue, log, 0);
}

/*!
 * @brief log_init function
 */
void log_init(uint32_t queue_length, uint32_t max_log_lenght)
{
    log_queue = xQueueCreate(queue_length, max_log_lenght);
    /* Enable queue view in MCUX IDE FreeRTOS TAD plugin. */
    if (log_queue != NULL)
    {
        vQueueAddToRegistry(log_queue, "LogQ");
    }
    if (xTaskCreate(log_task, "log_task", configMINIMAL_STACK_SIZE + 166, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
}

/*!
 * @brief log_print_task function
 */
static void log_task(void *pvParameters)
{
    uint32_t counter = 0;
    char log[MAX_LOG_LENGTH + 1];
    while (1)
    {
        if (xQueueReceive(log_queue, log, portMAX_DELAY) != pdTRUE)
        {
            PRINTF("Failed to receive queue.\r\n");
        }
        PRINTF("Log %d: %s\r\n", counter, log);
        counter++;
    }
}
