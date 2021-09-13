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
#include "HAL_timer_driver.h"
#include "isr_driver_intern.h"
#include "fsl_pint.h"
#include "fsl_inputmux.h"
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
        setupMode(EXT_INT_FD_PORT, callback->pin_func, callback->mode, callback->handler);
    }
}

void setupMode(PINT_Type *pint, ISR_PIN_FUNC_T pin, ISR_MODE_T mode, ISR_HANDLER_T handler)
{
    /* Connect trigger sources to PINT */
    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, I2C_FD_PIN_INT0_SRC);
    
    /* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX);
    /* Initialize PINT */
    PINT_Init(PINT);
    switch (mode)
    {
        case ISR_LEVEL_HI:
            PINT_PinInterruptConfig(pint, (pint_pin_int_t)pin, kPINT_PinIntEnableHighLevel,(pint_cb_t)handler);
            break;
        case ISR_LEVEL_LO:
            PINT_PinInterruptConfig(pint, (pint_pin_int_t)pin, kPINT_PinIntEnableLowLevel, (pint_cb_t)handler);
            break;
        case ISR_EDGE_RISE:
            PINT_PinInterruptConfig(pint, (pint_pin_int_t)pin, kPINT_PinIntEnableRiseEdge, (pint_cb_t)handler);
            break;
        case ISR_EDGE_FALL:
            PINT_PinInterruptConfig(pint, (pint_pin_int_t)pin, kPINT_PinIntEnableFallEdge, (pint_cb_t)handler);
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
void GINT0_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void GINT1_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PIN_INT0_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PIN_INT1_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PIN_INT2_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PIN_INT3_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PIN_INT4_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PIN_INT5_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PIN_INT6_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
void PIN_INT7_IRQHandler(void)
{
    SELECT_CALLBACK(0);
    CHECK_VALID_AND_CALL_HANDLER();
    WAKEUP_TASKS();
    DISABLE_INTERRUPT();
}
