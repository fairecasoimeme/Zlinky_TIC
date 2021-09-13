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
#include "board.h"
#include "fsl_port.h"
#include "HAL_timer_driver.h"
#include "isr_driver_intern.h"
/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
/***********************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                           */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL FUNCTIONS                                                    */
/***********************************************************************/

void HAL_ISR_Init()
{
    size_t i = 0;

    for (i = 0; i < ISR_MAX_CALLBACKS; i++)
    {
        SELECT_CALLBACK(i);

        /* disable interrupt */
        DISABLE_INTERRUPT();

        /* setup interrupt mode */
        setupMode(EXT_INT_FD_PORT, callback->pin_func, callback->mode);
    }
}

void setupMode(void *port, uint32_t pin, ISR_MODE_T mode)
{
    switch (mode)
    {
        case ISR_LEVEL_HI:
            PORT_SetPinInterruptConfig(port, pin, kPORT_InterruptLogicOne);
            break;
        case ISR_LEVEL_LO:
            PORT_SetPinInterruptConfig(port, pin, kPORT_InterruptLogicZero);
            break;
        case ISR_EDGE_RISE:
            PORT_SetPinInterruptConfig(port, pin, kPORT_InterruptRisingEdge);
            break;
        case ISR_EDGE_FALL:
            PORT_SetPinInterruptConfig(port, pin, kPORT_InterruptFallingEdge);
            break;
        default:
            break;
    }
}

void disableInterrupt(uint32_t channel)
{
    NVIC_DisableIRQ(GPIO_PININT_NVIC(channel));
}

void enableInterrupt(uint32_t channel)
{
    /* Enable interrupt in the NVIC */
    NVIC_ClearPendingIRQ(GPIO_PININT_NVIC(channel));
    NVIC_EnableIRQ(GPIO_PININT_NVIC(channel));
}

/***********************************************************************/
/* LOCAL FUNCTIONS                                                     */
/***********************************************************************/
/***********************************************************************/
/* INTERUPT SERVICE ROUTINES                                           */
/***********************************************************************/
void PORTA_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PORTB_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PORTB_PORTC_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PORTC_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PORTD_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PORTB_PORTC_PORTD_PORTE_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PORTC_PORTD_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
