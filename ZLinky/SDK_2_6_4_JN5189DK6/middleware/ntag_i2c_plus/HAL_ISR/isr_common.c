/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "isr_driver_intern.h"
#include "HAL_timer_driver.h"
/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
#undef ISR_CALLBACK_LIST_BEGIN
#undef ISR_CALLBACK_ENTRY
#undef ISR_CALLBACK_LIST_END

#define ISR_CALLBACK_LIST_BEGIN ISR_CALLBACK_T isr_callback_list[ISR_MAX_CALLBACKS] = {
#define ISR_CALLBACK_ENTRY(label, channel, pin_func)                                                        \
    {                                                                                                       \
        channel, pin_func, ISR_DEFAULT_MODE, ISR_INVALID_HANDLER, ISR_DEFAULT_ARGUMENT, ISR_SEMAPHORE_TAKEN \
    }
#define ISR_CALLBACK_LIST_END \
    }                         \
    ;

/***********************************************************************/
/* TYPES                                                               */
/***********************************************************************/
ISR_CALLBACK_LIST_BEGIN
#include "HAL_ISR_callback_list.h" /* allowed here - generator header */
ISR_CALLBACK_LIST_END

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
/***********************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                           */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL FUNCTIONS                                                    */
/***********************************************************************/
void HAL_ISR_RegisterCallback(ISR_SOURCE_T source, ISR_MODE_T mode, ISR_HANDLER_T handler, ISR_ARGUMENT_T arg)
{
    ISR_CALLBACK_T *callback = &isr_callback_list[(size_t)source];

    /* enter critical section */
    disableInterrupt(callback->channel);

    callback->mode = mode;
    callback->handler = handler;
    callback->arg = arg;

    ISR_SEMAPHORE_INIT(callback->sema);

    /* setup interrupt mode */
#if defined(__LPC8XX__) || defined(__LPC11UXX__) || defined(__LPC11U37H__) ||defined(CPU_LPC55S69JBD100_cm33_core0) ||defined(CPU_LPC5528JBD100)
    setupMode(EXT_INT_FD_PORT, callback->pin_func, callback->mode, callback->handler);
#else
    setupMode(EXT_INT_FD_PORT, callback->pin_func, callback->mode);
#endif
    /* leave critical section */
    enableInterrupt(callback->channel);
}

BOOL HAL_ISR_SleepWithTimeout(ISR_SOURCE_T source, uint32_t timeout_ms)
{
    SELECT_CALLBACK((size_t)source);
    uint32_t current = HAL_Timer_getTime_ms();
    uint32_t until = current + timeout_ms;

    do
    {
        /* start waiting for interrupt */
        enableInterrupt(callback->channel);

        ISR_WAIT_FOR_INTERRUPT();

        /* disable interrupt during check */
        disableInterrupt(callback->channel);
        current = HAL_Timer_getTime_ms();
    } while (ISR_SEMAPHORE_IS_TAKEN(callback->sema) && until > current);

    if (ISR_SEMAPHORE_IS_GIVEN(callback->sema))
    {
        /* reinit mode - necessary for edge sensitive on LPCxxx */
#if defined(__LPC8XX__) || defined(__LPC11UXX__) || defined(__LPC11U37H__) ||defined(CPU_LPC55S69JBD100_cm33_core0) || defined(CPU_LPC5528JBD100)
        setupMode(EXT_INT_FD_PORT, callback->pin_func, callback->mode, callback->handler);
#else
        setupMode(EXT_INT_FD_PORT, callback->pin_func, callback->mode);
#endif

        ISR_SEMAPHORE_TAKE(callback->sema);
        return FALSE;
    }

    return TRUE;
}

/***********************************************************************/
/* LOCAL FUNCTIONS                                                     */
/***********************************************************************/
/***********************************************************************/
/* INTERUPT SERVICE ROUTINES                                           */
/***********************************************************************/
