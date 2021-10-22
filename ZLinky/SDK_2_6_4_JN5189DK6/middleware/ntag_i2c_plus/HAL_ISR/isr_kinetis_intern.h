/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _ISR_FRDM_INTERN_H_
#define _ISR_FRDM_INTERN_H_

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include <board.h>
/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
#if defined(__LPC11UXX__) || defined(__LPC11U37H__)
#define GPIO_PININT_NVIC(channel) (PIN_INT0_IRQn + channel)
#define GPIO_PIN_MIN 0
#define GPIO_NUMBER_PINS 72
#endif /* LPC11U6x */
#if defined(FRDM_K82F)
#define GPIO_PININT_NVIC(channel) ((IRQn_Type)(PORTC_IRQn + channel))
#define EXT_INT_FD_GPIO GPIOC
#define EXT_INT_FD_PORT PORTC
#define EXT_INT_FD_PIN 10U
#endif /* FRDM_K82F */
#if defined(FRDM_K64F)
#define GPIO_PININT_NVIC(channel) ((IRQn_Type)(PORTA_IRQn + channel))
#define EXT_INT_FD_GPIO GPIOA
#define EXT_INT_FD_PORT PORTA
#define EXT_INT_FD_PIN 2U
#endif /* FRDM_K64F */
#if defined(HEXIWEAR)
#define GPIO_PININT_NVIC(channel) ((IRQn_Type)(PORTB_IRQn + channel))
#define EXT_INT_FD_GPIO GPIOB
#define EXT_INT_FD_PORT PORTB
#define EXT_INT_FD_PIN 13U
#endif /* FRDM_K64F */
#if defined(FRDM_KL27Z)
#define GPIO_PININT_NVIC(channel) ((IRQn_Type)(PORTB_PORTC_PORTD_PORTE_IRQn + channel))
#define EXT_INT_FD_GPIO GPIOE
#define EXT_INT_FD_PORT PORTE
#define EXT_INT_FD_PIN 24U
#endif /* FRDM_KL27Z */
#if defined(FRDM_KL43Z)
#define GPIO_PININT_NVIC(channel) ((IRQn_Type)(PORTA_IRQn + channel))
#define EXT_INT_FD_GPIO GPIOA
#define EXT_INT_FD_PORT PORTA
#define EXT_INT_FD_PIN 5U
#endif /* FRDM_KL43Z */
#if defined(FRDM_K22F)
#define GPIO_PININT_NVIC(channel) ((IRQn_Type)(PORTB_IRQn + channel))
#define EXT_INT_FD_GPIO GPIOB
#define EXT_INT_FD_PORT PORTB
#define EXT_INT_FD_PIN 18U
#endif /* FRDM_K22F */
#if defined(FRDM_KL02Z)
#define GPIO_PININT_NVIC(channel) ((IRQn_Type)(PORTA_IRQn + channel))
#define EXT_INT_FD_GPIO GPIOA
#define EXT_INT_FD_PORT PORTA
#define EXT_INT_FD_PIN 12U
#endif /* FRDM_KL02Z */
#if defined(FRDM_KW41Z)
#define GPIO_PININT_NVIC(channel) ((IRQn_Type)(PORTB_PORTC_IRQn + channel))
#define EXT_INT_FD_GPIO GPIOC
#define EXT_INT_FD_PORT PORTC
#define EXT_INT_FD_PIN 17U
#endif /* FRDM_KW41Z */

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
