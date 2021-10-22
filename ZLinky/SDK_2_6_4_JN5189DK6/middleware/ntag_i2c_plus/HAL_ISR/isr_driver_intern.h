/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _ISR_DRIVER_INTERN_H_
#define _ISR_DRIVER_INTERN_H_

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "HAL_ISR_driver.h"

#if defined(__LPC8XX__) || defined(__LPC11UXX__) || defined(__LPC11U37H__) ||defined(CPU_LPC55S69JBD100_cm33_core0) || defined(CPU_LPC5528JBD100)
#include "isr_lpc_intern.h"
#include "fsl_pint.h"
#endif
#if defined(FRDM_K82F) || defined(FRDM_K64F) || defined(FRDM_KL27Z) || defined(FRDM_KL43Z) || defined(FRDM_K22F) || \
    defined(FRDM_KL02Z) || defined(FRDM_KW41Z) || defined(HEXIWEAR)
#include "isr_kinetis_intern.h"
#endif

/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
#define ISR_DEFAULT_MODE ISR_EDGE_RISE

#define SELECT_CALLBACK(index) ISR_CALLBACK_T *callback = &isr_callback_list[index];

#define CHECK_VALID_AND_CALL_HANDLER()            \
    if (callback->handler != ISR_INVALID_HANDLER) \
        callback->handler(callback->arg);

#define WAKEUP_TASKS() ISR_SEMAPHORE_GIVE(callback->sema)

#define DISABLE_INTERRUPT() disableInterrupt(callback->channel)

/***********************************************************************/
/* TYPES                                                               */
/***********************************************************************/
typedef uint32_t ISR_CHANNEL_T;
typedef uint32_t ISR_PIN_FUNC_T;

typedef struct
{
    ISR_CHANNEL_T channel;
    ISR_PIN_FUNC_T pin_func;
    ISR_MODE_T mode;
    ISR_ARGUMENT_T arg;
    ISR_SEMAPHORE_T sema;
    ISR_HANDLER_T handler;
} ISR_CALLBACK_T;

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
extern ISR_CALLBACK_T isr_callback_list[];

/***********************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                          */
/***********************************************************************/
void disableInterrupt(uint32_t channel);
void enableInterrupt(uint32_t channel);
#if defined(__LPC8XX__) || defined(__LPC11UXX__) || defined(__LPC11U37H__) || defined(CPU_LPC55S69JBD100_cm33_core0) || defined(CPU_LPC5528JBD100)
void setupMode(PINT_Type *pint, ISR_PIN_FUNC_T pin, ISR_MODE_T mode, ISR_HANDLER_T callback);
#else
void setupMode(void *port, uint32_t pin, ISR_MODE_T mode);
#endif

#endif /* _ISR_DRIVER_INTERN_H_ */
