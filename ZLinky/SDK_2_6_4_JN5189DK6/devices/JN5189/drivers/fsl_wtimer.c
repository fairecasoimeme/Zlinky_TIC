/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include "fsl_wtimer.h"
#include "fsl_clock.h"
#include "fsl_device_registers.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.wtimer"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

//#define WTIMER_TRACE

#ifndef WTIMER_TRACE
#define PRINTF(...)
#else
#include "fsl_debug_console.h"
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.wtimer"
#endif
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

typedef struct
{
    uint32_t wkt_stat_timeout_mask;
    uint32_t wkt_stat_running_mask;
    uint32_t wkt_ctrl_clk_ena_mask;
    uint32_t wkt_ctrl_ena_mask;
    uint32_t wkt_intenclr_timeout_mask;
    volatile uint32_t *wkt_load_lsb_reg;
    volatile uint32_t *wkt_load_msb_reg;
    const volatile uint32_t *wkt_val_lsb_reg;
    const volatile uint32_t *wkt_val_msb_reg;
    const uint32_t wkt_val_lsb_mask;
    const uint32_t wkt_val_msb_mask;
    uint8_t wkt_irq_id;
} timer_param_t;

static const timer_param_t timer_param[2] = {
    {
        .wkt_stat_timeout_mask     = SYSCON_WKT_STAT_WKT0_TIMEOUT_MASK,
        .wkt_stat_running_mask     = SYSCON_WKT_STAT_WKT0_RUNNING_MASK,
        .wkt_ctrl_clk_ena_mask     = SYSCON_WKT_CTRL_WKT0_CLK_ENA_MASK,
        .wkt_ctrl_ena_mask         = SYSCON_WKT_CTRL_WKT0_ENA_MASK,
        .wkt_intenclr_timeout_mask = SYSCON_WKT_INTENSET_WKT0_TIMEOUT_MASK,
        .wkt_load_lsb_reg          = &SYSCON->WKT_LOAD_WKT0_LSB,
        .wkt_load_msb_reg          = &SYSCON->WKT_LOAD_WKT0_MSB,
        .wkt_val_lsb_reg           = &SYSCON->WKT_VAL_WKT0_LSB,
        .wkt_val_msb_reg           = &SYSCON->WKT_VAL_WKT0_MSB,
        .wkt_val_lsb_mask          = SYSCON_WKT_LOAD_WKT0_LSB_WKT0_LOAD_LSB_MASK,
        .wkt_val_msb_mask          = SYSCON_WKT_LOAD_WKT0_MSB_WKT0_LOAD_MSB_MASK,
        .wkt_irq_id                = WAKE_UP_TIMER0_IRQn,
    },
    {
        .wkt_stat_timeout_mask     = SYSCON_WKT_STAT_WKT1_TIMEOUT_MASK,
        .wkt_stat_running_mask     = SYSCON_WKT_STAT_WKT1_RUNNING_MASK,
        .wkt_ctrl_clk_ena_mask     = SYSCON_WKT_CTRL_WKT1_CLK_ENA_MASK,
        .wkt_ctrl_ena_mask         = SYSCON_WKT_CTRL_WKT1_ENA_MASK,
        .wkt_intenclr_timeout_mask = SYSCON_WKT_INTENSET_WKT1_TIMEOUT_MASK,
        .wkt_load_lsb_reg          = &SYSCON->WKT_LOAD_WKT1,
        .wkt_load_msb_reg          = NULL,
        .wkt_val_lsb_reg           = &SYSCON->WKT_VAL_WKT1,
        .wkt_val_msb_reg           = NULL,
        .wkt_val_lsb_mask          = SYSCON_WKT_LOAD_WKT0_LSB_WKT0_LOAD_LSB_MASK,
        .wkt_val_msb_mask          = 0,
        .wkt_irq_id                = WAKE_UP_TIMER1_IRQn,
    },
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief Enable the clocks to the peripheral (functional clock and AHB clock)
 *
 * note This function does not reset the wake timer peripheral. Wake timer reset is done in PWRM_vColdStart() from the
 * PWRM framework module if integrated
 * If PWRM framework module is integrated, WTIMER_Init() is called in PWRM_vInit() for power modes with Oscillator ON.
 *
 */
void WTIMER_Init(void)
{
    if ((0 == (SYSCON->AHBCLKCTRLS[0] & SYSCON_AHBCLKCTRLSET0_WAKE_UP_TIMERS_CLK_SET_MASK)) ||
        (0 != (SYSCON->WKTCLKSEL & SYSCON_WKTCLKSEL_SEL_MASK)))
    {
        /* set clock and divider */
        SYSCON->AHBCLKCTRLS[0] |= SYSCON_AHBCLKCTRLSET0_WAKE_UP_TIMERS_CLK_SET_MASK;
        SYSCON->WKTCLKSEL = SYSCON_WKTCLKSEL_SEL(0);
    }
}

/*!
 * brief Disable the clocks to the peripheral (functional clock and AHB clock)
 *
 * note This function does not reset the wake timer peripheral.
 *
 */
void WTIMER_DeInit(void)
{
    if ((SYSCON->WKT_CTRL & (SYSCON_WKT_CTRL_WKT1_ENA_MASK | SYSCON_WKT_CTRL_WKT0_ENA_MASK)) != 0)
    {
       /* set clock and divider */
        SYSCON->AHBCLKCTRLS[0] &= ~SYSCON_AHBCLKCTRLSET0_WAKE_UP_TIMERS_CLK_SET_MASK;
        SYSCON->WKTCLKSEL = SYSCON_WKTCLKSEL_SEL(2); /* No Clock */
    }

}

/*!
 * brief Gets the Timer status flags.
 *
 * param timer_id   Wtimer Id
 *
 * return The status flags.
 */
WTIMER_status_t WTIMER_GetStatusFlags(WTIMER_timer_id_t timer_id)
{
    const timer_param_t *timer_param_l = &timer_param[timer_id];
    WTIMER_status_t status             = WTIMER_STATUS_NOT_RUNNING;
    uint32_t stat                      = SYSCON->WKT_STAT;

    if (stat & timer_param_l->wkt_stat_timeout_mask)
    {
        status = WTIMER_STATUS_EXPIRED;
        PRINTF("WakeTimerFiredStatus[%d] expired\n", timer_id);
    }
    else if (stat & timer_param_l->wkt_stat_running_mask)
    {
        status = WTIMER_STATUS_RUNNING;
        PRINTF("WakeTimerFiredStatus[%d] running\n", timer_id);
    }

    return status;
}

/*!
 * brief Enable the selected Timer interrupts.
 * The application shall implement the Wake timer ISR
 *
 * param timer_id   Wtimer Id
 */
void WTIMER_EnableInterrupts(WTIMER_timer_id_t timer_id)
{
    const timer_param_t *timer_param_l = &timer_param[timer_id];

    EnableIRQ((IRQn_Type)timer_param_l->wkt_irq_id);
    SYSCON->WKT_INTENSET = timer_param_l->wkt_intenclr_timeout_mask;
}

static void WTIMER_StartTimerFunction(WTIMER_timer_id_t timer_id, uint64_t count, bool fast)
{
    const timer_param_t *timer_param_l = &timer_param[timer_id];

    PRINTF("-->> vAHI_WakeTimerStart[%d] : STAT=%x count=%d WKT_INTSTAT=%x count=%d\n", timer_id, SYSCON->WKT_STAT,
           count, SYSCON->WKT_INTSTAT, count);
    /* enable the clock */
    SYSCON->WKT_CTRL |= timer_param_l->wkt_ctrl_clk_ena_mask;

    /* Check if the timer is enabled */
    if ( (!fast) || (SYSCON->WKT_CTRL & timer_param_l->wkt_ctrl_ena_mask))
    {
        /* clear timeout flag if set */
        SYSCON->WKT_STAT = timer_param_l->wkt_stat_timeout_mask;

        /* stop timer if running */
        SYSCON->WKT_CTRL &= ~(timer_param_l->wkt_ctrl_ena_mask);

        /* make sure the timer is really stopped */
        while ((SYSCON->WKT_STAT & (timer_param_l->wkt_stat_running_mask)) == timer_param_l->wkt_stat_running_mask)
        {
            __asm volatile("nop");
        }
    }

    uint32_t lsb_count = (count & 0xffffffff);

    *(timer_param_l->wkt_load_lsb_reg) = lsb_count  & timer_param_l->wkt_val_lsb_mask;
    if (timer_id == WTIMER_TIMER0_ID)
    {
        uint32_t msb_count = (uint32_t)((uint64_t)count>>32);
        *(timer_param_l->wkt_load_msb_reg) = msb_count & timer_param_l->wkt_val_msb_mask;
    }

    if (fast)
    {
        /* enable the timer */
        SYSCON->WKT_CTRL |= timer_param_l->wkt_ctrl_ena_mask;
    }
    else
    {
        /* handle as a critical  section because if an interrupt that triggers immediately after the timer enable can cause
           a lock in the while below if the interrupt calls WTIMER_StopTimer */
        DisableIRQ((IRQn_Type)timer_param_l->wkt_irq_id);
        /* enable the timer */
        SYSCON->WKT_CTRL |= timer_param_l->wkt_ctrl_ena_mask;

        while ((SYSCON->WKT_STAT & (timer_param_l->wkt_stat_running_mask)) == 0)
        {
            __asm volatile("nop");
        }
        EnableIRQ((IRQn_Type)timer_param_l->wkt_irq_id);
    }

    PRINTF("<<-- vAHI_WakeTimerStart[%d] : STAT=%x WKT_INTSTAT=%x\n", timer_id, SYSCON->WKT_STAT, SYSCON->WKT_INTSTAT);
}

/*!
 * brief Starts the Timer counter.
 * The function performs:
 * -stop the timer if running, clear the status and interrupt flag if set (WTIMER_ClearStatusFlags())
 * -set the counter value
 * -start the timer
 *
 * param timer_id   Wtimer Id
 * param msb_count      number of 32KHz clock periods before expiration divided by 2^32
 * param lsb_count      number of 32KHz clock periods before expiration modulo 2^32
 *
 */
void WTIMER_StartTimerLarge(WTIMER_timer_id_t timer_id, uint64_t count)
{
    WTIMER_StartTimerFunction(timer_id, count, false);
}



/*!
 * brief Starts the Timer counter.
 * The function performs:
 * -stop the timer if running, clear the status and interrupt flag if set (WTIMER_ClearStatusFlags())
 * -set the counter value
 * -start the timer
 *
 * param timer_id   Wtimer Id
 * param count      number of 32KHz clock periods before expiration
 */
void WTIMER_StartTimer(WTIMER_timer_id_t timer_id, uint32_t count)
{
    WTIMER_StartTimerFunction(timer_id, (uint64_t)count, false);
}

/*!
 * brief Starts the Timer counter without waiting for the WKT_STAT register to be loaded.
 *
 * The function performs:
 * -stop the timer if running, clear the status and interrupt flag if set (WTIMER_ClearStatusFlags())
 * -set the counter value
 * -start the timer
 *
 * param timer_id   Wtimer Id
 * param count      number of 32KHz clock periods before expiration
 */
void WTIMER_StartTimerUnsafe(WTIMER_timer_id_t timer_id, uint32_t count)
{
    WTIMER_StartTimerFunction(timer_id, (uint64_t)count, true);
}

/*!
 * brief Read the LSB counter of the wake timer
 * API checks the next counter update (next 32KHz clock edge) so the value is uptodate
 * Important note : The counter shall be running otherwise, the API gets locked and never return
 *
 * param timer_id   Wtimer Id
 * return  32KHz clock frequency (number of 32KHz clock in one sec) - expect to have 32768
 */
uint32_t WTIMER_ReadTimerSafe(WTIMER_timer_id_t timer_id)
{
    const timer_param_t *timer_param_l    = &timer_param[timer_id];
    volatile uint32_t u32CurrentValue = *timer_param_l->wkt_val_lsb_reg;
    volatile uint32_t u32NextValue   = *timer_param_l->wkt_val_lsb_reg;
    uint8_t equalFound = 0;

    while (u32NextValue == u32CurrentValue)
    {
        u32NextValue = *timer_param_l->wkt_val_lsb_reg;
    }
    /* Now wait a stable value */
    while (equalFound < 5)
    {
        u32NextValue = *timer_param_l->wkt_val_lsb_reg;
        if (u32NextValue == u32CurrentValue)
        {
            equalFound++;
        }
        u32CurrentValue = u32NextValue;
    }
    return u32CurrentValue;
}

/*!
 * brief Read the LSB counter of the wake timer
 * This API is unsafe. If the counter has just been started, the counter value may not be
 * up to date until the next 32KHz clock edge.
 * Use WTIMER_ReadTimerSafe() instead
 *
 * param timer_id   Wtimer Id
 * return  counter value - number of ticks before expiration if running
 */
uint32_t WTIMER_ReadTimer(WTIMER_timer_id_t timer_id)
{
    const timer_param_t *timer_param_l = &timer_param[timer_id];
    volatile uint32_t u32CurrentCount;
    uint32_t u32PrevCount = 0;
    int loopcnt = 0;
    do {
    	 u32CurrentCount = *timer_param_l->wkt_val_lsb_reg;
    	 /* the counter is counting down, previous value might be greater than current value by 1
    	  * in which case it is 'expected'. Otherwise restart stabilization count.
    	  * */
    	 if ((u32PrevCount - u32CurrentCount) > 1)
    	 {
    		 loopcnt = 0;
    		 u32PrevCount = u32CurrentCount;
    	 }
    	 loopcnt++;

    } while (loopcnt < 5);
    return u32CurrentCount;
}

/*!
 * brief Clears the Timer status flags if expired and clear the pendng interrupt if active
 * it needs to be called in ISR
 *
 * param timer_id   Wtimer Id
 */
void WTIMER_ClearStatusFlags(WTIMER_timer_id_t timer_id)
{
    const timer_param_t *timer_param_l = &timer_param[timer_id];

    /* clear expiration flag */
    SYSCON->WKT_STAT = timer_param_l->wkt_stat_timeout_mask;


    /* clear interrupt if pending */
    NVIC_ClearPendingIRQ((IRQn_Type)timer_param_l->wkt_irq_id);
}


static void WTIMER_StopTimerFunction(WTIMER_timer_id_t timer_id, bool fast)
{
    const timer_param_t *timer_param_l = &timer_param[timer_id];

    /* Stop timer */
    SYSCON->WKT_CTRL &= ~(timer_param_l->wkt_ctrl_ena_mask);

    if (!fast)
    {
        /* make sure the timer is really stopped */
        while ((SYSCON->WKT_STAT & (timer_param_l->wkt_stat_running_mask)) == timer_param_l->wkt_stat_running_mask)
        {
            __asm volatile("nop");
        }
    }

    WTIMER_ClearStatusFlags(timer_id);
}

/*!
 * brief Stops the Timer counter without waiting for the WKT_STAT register to be loaded
 *
 * param timer_id   Wtimer Id
 */
void WTIMER_StopTimerUnsafe(WTIMER_timer_id_t timer_id)
{
    WTIMER_StopTimerFunction(timer_id, true);
}

/*!
 * brief Stops the Timer counter.
 *
 * param timer_id   Wtimer Id
 */
void WTIMER_StopTimer(WTIMER_timer_id_t timer_id)
{
    WTIMER_StopTimerFunction(timer_id, false);
}

/*!
 * brief Read the LSB counter of the wake timer
 * API checks the next counter update (next 32KHz clock edge) so the value is uptodate
 * Important note : The counter shall be running otherwise, the API gets locked and never return
 *
 * param timer_id   Wtimer Id
 * return  Number of 32kHz clock ticks recombined from values read in LSB and MSB registers for WTIMER0.
 *         LSB ony for WTIMER1
 */
uint64_t WTIMER_ReadTimerLargeSafe(WTIMER_timer_id_t timer_id)
{
    uint64_t ts;
    const timer_param_t *timer_param_l    = &timer_param[timer_id];
    volatile uint32_t lsb32_ini = *timer_param_l->wkt_val_lsb_reg;
    volatile uint32_t lsb32     = *timer_param_l->wkt_val_lsb_reg;

    while (lsb32 == lsb32_ini)
        lsb32 = *timer_param_l->wkt_val_lsb_reg;


    if (timer_id == WTIMER_TIMER0_ID)
    {
        uint32_t msb32 = *timer_param_l->wkt_val_msb_reg;
        lsb32 = *timer_param_l->wkt_val_lsb_reg;
        if (lsb32 == 0)
        {
            /* read MSB again: it may have changed when propagating the carry */
             msb32 = *timer_param_l->wkt_val_msb_reg;
        }

        ts = ((uint64_t)(msb32 & timer_param_l->wkt_val_msb_mask) << 32) | lsb32;
    }
    else
    {
        ts  = (uint64_t)lsb32;
    }

    return ts;
}

uint64_t WTIMER_ReadTimerLarge(WTIMER_timer_id_t timer_id)
{
    uint64_t ts;
    const timer_param_t *timer_param_l = &timer_param[timer_id];

    if (timer_id == WTIMER_TIMER0_ID)
    {
        uint32_t msb32 = *timer_param_l->wkt_val_msb_reg;
        uint32_t lsb32 = *timer_param_l->wkt_val_lsb_reg;
        if (lsb32 == 0)
        {
            /* read MSB again: it may have changed when propagating the carry */
             msb32 = *timer_param_l->wkt_val_msb_reg;
        }

        ts = ((uint64_t)(msb32 & timer_param_l->wkt_val_msb_mask) << 32) | lsb32;
    }
    else
    {
        ts  = (uint64_t)WTIMER_ReadTimer(timer_id);
    }

    return ts;

}
