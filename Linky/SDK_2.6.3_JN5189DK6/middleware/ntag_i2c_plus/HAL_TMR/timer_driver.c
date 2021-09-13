/*
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "HAL_timer_driver.h"
#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "fsl_common.h"

/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
#if !defined(FSL_RTOS_FREE_RTOS)
static volatile uint32_t timer = 0;
#endif

/***********************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                           */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL FUNCTIONS                                                    */
/***********************************************************************/
void HAL_Timer_Init()
{
#if !defined(FSL_RTOS_FREE_RTOS)
    /* produce a timer interrupt every 1ms */
    SysTick_Config(SystemCoreClock / 1000);
#endif
}

void HAL_Timer_delay_ms(uint32_t ms)
{
#if !defined(FSL_RTOS_FREE_RTOS)
    volatile uint32_t until = timer + ms;
    while (until > timer)
    {
        __WFI();
    }
#else
    vTaskDelay(pdMS_TO_TICKS(ms));
#endif    
}

uint32_t HAL_Timer_getTime_ms()
{
#if !defined(FSL_RTOS_FREE_RTOS)
    return timer;
#else
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
#endif    
}

/***********************************************************************/
/* LOCAL FUNCTIONS                                                     */
/***********************************************************************/
/***********************************************************************/
/* INTERUPT SERVICE ROUTINES                                           */
/***********************************************************************/
#if !defined(FSL_RTOS_FREE_RTOS)
void SysTick_Handler(void)
{
    timer++;
}
#endif

