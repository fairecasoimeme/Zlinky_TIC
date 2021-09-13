/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_rtc.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_clock.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void delay(uint32_t d);


/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile bool busyWait;

/*******************************************************************************
 * Code
 ******************************************************************************/


/*!
 * @brief ISR for Alarm interrupt
 *
 * This function changes the state of busyWait.
 */
void RTC_IRQHandler(void)
{
    if (RTC_GetStatusFlags(RTC) & kRTC_AlarmFlag)
    {
        busyWait = false;

        /* Clear alarm flag */
        RTC_ClearStatusFlags(RTC, kRTC_AlarmFlag);
    }
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t sec;
    uint32_t currSeconds;
    uint8_t index;
    rtc_datetime_t date;

    /* Board pin, clock, debug console init */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* Init the boards */
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    /* RTC clocks dividers */
    CLOCK_SetClkDiv(kCLOCK_DivRtcClk, 32, false);
    CLOCK_SetClkDiv(kCLOCK_DivRtc1HzClk, 1, false);

    CLOCK_uDelay(10000);

    /* Init RTC */
    RTC_Init(RTC);

    PRINTF("RTC example: set up time to wake up an alarm\r\n");

    /* Set a start date time and start RT */
    date.year   = 2014U;
    date.month  = 12U;
    date.day    = 25U;
    date.hour   = 19U;
    date.minute = 0;
    date.second = 0;

    /* RTC time counter has to be stopped before setting the date & time in the TSR register */
    RTC_StopTimer(RTC);

    /* Set RTC time to default */
    RTC_SetDatetime(RTC, &date);

    /* Enable RTC alarm interrupt */
    RTC_EnableInterrupts(RTC, kRTC_AlarmInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(RTC_IRQn);

    /* Start the RTC time counter */
    RTC_StartTimer(RTC);

    /* This loop will set the RTC alarm */
    while (1)
    {
        busyWait = true;
        index    = 0;
        sec      = 0;
        /* Get date time */
        RTC_GetDatetime(RTC, &date);

        /* print default time */
        PRINTF("Current datetime: %04d-%02d-%02d %02d:%02d:%02d\r\n", date.year, date.month, date.day, date.hour,
               date.minute, date.second);

        /* Get alarm time from user */
        PRINTF("Please input the number of second to wait for alarm \r\n");
        PRINTF("The second must be positive value\r\n");
        while (index != 0x0D)
        {
            index = GETCHAR();
            if ((index >= '0') && (index <= '9'))
            {
                PUTCHAR(index);
                sec = sec * 10 + (index - 0x30U);
            }
        }
        PRINTF("\r\n");

        /* Read the RTC seconds register to get current time in seconds */
        currSeconds = RTC->COUNT;

        /* Add alarm seconds to current time */
        currSeconds += sec;

        /* Set alarm time in seconds */
        RTC->MATCH = currSeconds;

        /* Get alarm time */
        RTC_GetAlarm(RTC, &date);

        /* Print alarm time */
        PRINTF("Alarm will occur at: %04d-%02d-%02d %02d:%02d:%02d\r\n", date.year, date.month, date.day, date.hour,
               date.minute, date.second);

        /* Wait until alarm occurs */
        while (busyWait)
        {
        }

        PRINTF("\r\n Alarm occurs !!!! ");
    }
}
