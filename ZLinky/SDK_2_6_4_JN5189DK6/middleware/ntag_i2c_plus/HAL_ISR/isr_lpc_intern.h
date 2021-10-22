/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _ISR_LPC_INTERN_H_
#define _ISR_LPC_INTERN_H_

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include <board.h>
/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/

#define GPIO_PININT_NVIC(channel) (IRQn_Type)(PIN_INT0_IRQn + channel)
#define EXT_INT_FD_PORT PINT
//#define GPIO_PIN_MIN 0
//#define GPIO_NUMBER_PINS 72
#define I2C_FD_PIN_INT0_SRC kINPUTMUX_GpioPort0Pin5ToPintsel

#define ISR_PIN_FUNC_INVALID 0xFFFFFFFF

#define ISR_SEMAPHORE_GIVEN true
#define ISR_SEMAPHORE_TAKEN false

#define ISR_SEMAPHORE_INIT(x)    \
    do                           \
    {                            \
        x = ISR_SEMAPHORE_TAKEN; \
    } while (0)
#define ISR_SEMAPHORE_GIVE(x)    \
    do                           \
    {                            \
        x = ISR_SEMAPHORE_GIVEN; \
    } while (0)
#define ISR_SEMAPHORE_GIVE_FROM_ISR(x) \
    do                                 \
    {                                  \
        x = ISR_SEMAPHORE_GIVEN;       \
    } while (0)

#define ISR_SEMAPHORE_TAKE(x) \
    do                        \
    {                         \
        x = false;            \
    } while (0)

#define ISR_SEMAPHORE_IS_GIVEN(x) (x == ISR_SEMAPHORE_GIVEN)
#define ISR_SEMAPHORE_IS_TAKEN(x) (x == ISR_SEMAPHORE_TAKEN)

#define ISR_WAIT_FOR_INTERRUPT() __WFI();

/***********************************************************************/
/* TYPES                                                               */
/***********************************************************************/
typedef BOOL ISR_SEMAPHORE_T;

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                          */
/***********************************************************************/
#endif /* _ISR_FRDM_INTERN_H_ */
