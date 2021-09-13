/****************************************************************************
 *
 * Copyright 2020 NXP.
 *
 * NXP Confidential. 
 * 
 * This software is owned or controlled by NXP and may only be used strictly 
 * in accordance with the applicable license terms.  
 * By expressly accepting such terms or by downloading, installing, activating 
 * and/or otherwise using the software, you are agreeing that you have read, 
 * and that you agree to comply with and are bound by, such license terms.  
 * If you do not agree to be bound by the applicable license terms, 
 * then you may not retain, install, activate or otherwise use the software. 
 * 
 *
 ****************************************************************************/


/*****************************************************************************
 *
 * MODULE:             ZTimer
 *
 * COMPONENT:          ZTimer.c
 *
 * DESCRIPTION:        ZigBee Timer Module
 *
 ****************************************************************************/

/****************************************************************************/
/*          Include files                                                   */
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "dbg.h"
#include "ZTimer.h"
#include "pwrm.h"
#include "MicroSpecific.h"
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    #include "AppHardwareApi.h"
#else
    #include "fsl_os_abstraction.h"
    #ifndef FSL_RTOS_FREE_RTOS
        #include "fsl_os_abstraction_bm.h"
    #else
        #include "fsl_os_abstraction_free_rtos.h"
    #endif
#endif


/****************************************************************************/
/*          Macro Definitions                                               */
/****************************************************************************/

#ifdef DEBUG_ZTIMER
#define TRACE_ZTIMER    TRUE
#else
#define TRACE_ZTIMER    FALSE
#endif

/****************************************************************************/
/***        Type Definitions                                                */
/****************************************************************************/

typedef struct
{

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    volatile uint8     u8Ticks;
#else
    volatile uint32   u32Ticks;
#endif
    uint8            u8NumTimers;
    ZTIMER_tsTimer    *psTimers;
} ZTIMER_tsCommon;

/****************************************************************************/
/*          Local Function Prototypes                                       */
/****************************************************************************/

/****************************************************************************/
/*          Exported Variables                                              */
/****************************************************************************/

/****************************************************************************/
/*          Local Variables                                                 */
/****************************************************************************/

PRIVATE ZTIMER_tsCommon ZTIMER_sCommon;

/****************************************************************************/
/*          Exported Functions                                              */
/****************************************************************************/

/****************************************************************************
 *
 * NAME: ZTIMER_eInit
 *
 * DESCRIPTION:
 * Initialises the ZTimer module
 *
 * RETURNS:
 * ZTIMER_teStatus
 *
 ****************************************************************************/
PUBLIC ZTIMER_teStatus ZTIMER_eInit(ZTIMER_tsTimer *psTimers, uint8 u8NumTimers)
{

    DBG_vPrintf(TRACE_ZTIMER, "ZT: Initialising: ");

    if(psTimers == NULL || u8NumTimers == 0)
    {
        DBG_vPrintf(TRACE_ZTIMER, "Failed\n");
        return E_ZTIMER_FAIL;
    }
	ZTIMER_sCommon.u8NumTimers = u8NumTimers;
    ZTIMER_sCommon.psTimers = psTimers;
    #if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
        ZTIMER_sCommon.u8Ticks = 0;	
    #else
        ZTIMER_sCommon.u32Ticks = 0;
    #endif
    
    memset(psTimers, 0, sizeof(ZTIMER_tsTimer) * u8NumTimers);

    #if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
        vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);
        #if(defined JENNIC_CHIP_FAMILY_JN516x)
            vAHI_TickTimerWrite(0);
        #endif

        #if (defined JENNIC_CHIP_FAMILY_JN517x)
            vAHI_TickTimerClear();
            vAHI_TickTimerInit(ZTIMER_vAhiCallback);
        #endif

        vAHI_TickTimerInterval(16000);
        vAHI_TickTimerIntEnable(TRUE);

        vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_RESTART);

    #endif

    DBG_vPrintf(TRACE_ZTIMER, "Success\n");

    return E_ZTIMER_OK;

}


/****************************************************************************
 *
 * NAME: ZTIMER_cbCallback
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
#if (defined JENNIC_CHIP_FAMILY_JN516x)
PUBLIC void ISR_vTickTimer(void)
{
    vAHI_TickTimerIntPendClr();
    if(ZTIMER_sCommon.u8Ticks < 0xff)
    {
        ZTIMER_sCommon.u8Ticks++;
    }
}

#elif (defined JENNIC_CHIP_FAMILY_JN517x)
PUBLIC void ZTIMER_vAhiCallback ( uint32 u32Device, uint32 u32ItemBitmap)
{
    if(ZTIMER_sCommon.u8Ticks < 0xff)
    {
        ZTIMER_sCommon.u8Ticks++;
    }
}
#endif
/****************************************************************************
 *
 * NAME: ZTIMER_vSleep
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void ZTIMER_vSleep(void)
{
    #if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
        vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);
    #else
        SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
    #endif

}


/****************************************************************************
 *
 * NAME: ZTIMER_vWake
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void ZTIMER_vWake(void)
{

    #if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
        vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);
        #if (defined JENNIC_CHIP_FAMILY_JN516x)
            vAHI_TickTimerWrite(0);
        #endif
        #if (defined JENNIC_CHIP_FAMILY_JN517x)
            vAHI_TickTimerClear();
            vAHI_TickTimerInit(ZTIMER_vAhiCallback);
        #endif
        vAHI_TickTimerInterval(16000);
        vAHI_TickTimerIntEnable(TRUE);
        vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_RESTART);
    #else
        extern void OSA_TimeInit(void);
        OSA_TimeInit();
    #endif

}


/****************************************************************************
 *
 * NAME: ZTIMER_vTask
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void ZTIMER_vTask(void)
{

    int n;
    ZTIMER_tsTimer *psTimer;
	uint32 u32Store;

#if (defined JENNIC_CHIP_FAMILY_JN518x)
    /* save the old tick */
    volatile uint32 u32Tick_old = ZTIMER_sCommon.u32Ticks;
    /* get new tick */
    volatile uint32 u32Tick_new = OSA_TimeGetMsec();
    /* Get number of ticks since last */
    
    if(u32Tick_new >= u32Tick_old)
    {
        ZTIMER_sCommon.u32Ticks = u32Tick_new - u32Tick_old;
    }
    else
    {
       /*recover from roll over*/
       ZTIMER_sCommon.u32Ticks = FSL_OSA_TIME_RANGE - u32Tick_old;
       ZTIMER_sCommon.u32Ticks += u32Tick_new;
    }
    if(ZTIMER_sCommon.u32Ticks == 0)
    {
        ZTIMER_sCommon.u32Ticks = u32Tick_new;
        return;
    }
#else
    /* If no ticks to process, exit */
    if(ZTIMER_sCommon.u8Ticks == 0)
    {
        return;
    }

    /* Decrement the tick counter */
    ZTIMER_sCommon.u8Ticks--;
#endif
    DBG_vPrintf(TRACE_ZTIMER, "ZT: Tick\n");

    /* Process all of the timers */
    for(n = 0; n < ZTIMER_sCommon.u8NumTimers; n++)
    {

        psTimer = &ZTIMER_sCommon.psTimers[n];

        /* If this timer is not opened and running, skip it */
        if(psTimer->eState != E_ZTIMER_STATE_RUNNING)
        {
            continue;
        }

        DBG_vPrintf(TRACE_ZTIMER, "ZT: Processing timer %d - time %d\n", n, psTimer->u32Time);

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
        /* Decrement the time */
        psTimer->u32Time--;

        /* If the timer has not expired, move on to the next one */
        if(psTimer->u32Time > 0)
        {
            continue;
        }
#else
        if(psTimer->u32Time > ZTIMER_sCommon.u32Ticks)
       {
            psTimer->u32Time -= ZTIMER_sCommon.u32Ticks;
            continue;
       }

#endif
        DBG_vPrintf(TRACE_ZTIMER, "ZT: Timer %d expired\n", n);
#if ZIGBEE_USE_FRAMEWORK
        OSA_InterruptEnableRestricted(&u32Store);
#else
        MICRO_DISABLE_AND_SAVE_INTERRUPTS(u32Store);
#endif
        /* Mark the timer as expired. We must do this _before_ calling the callback
         * in case the user restarts the timer in the callback */
        psTimer->eState = E_ZTIMER_STATE_EXPIRED;

        

        /* If this timer should prevent sleeping while running, decrement the activity count */
        if(psTimer->u8Flags & ZTIMER_FLAG_PREVENT_SLEEP)
        {
            PWRM_eFinishActivity();        
        }
#if ZIGBEE_USE_FRAMEWORK
        OSA_InterruptEnableRestore(&u32Store);
#else
        MICRO_RESTORE_INTERRUPTS(u32Store);
#endif
        /* If the timer has  a valid callback, call it */
        if(psTimer->pfCallback != NULL)
        {
            psTimer->pfCallback(psTimer->pvParameters);
        }

    }

#if (defined JENNIC_CHIP_FAMILY_JN518x)
    ZTIMER_sCommon.u32Ticks = u32Tick_new;
#endif

}


/****************************************************************************
 *
 * NAME: ZTIMER_eOpen
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * ZTIMER_teStatus
 *
 ****************************************************************************/
PUBLIC ZTIMER_teStatus ZTIMER_eOpen(uint8 *pu8TimerIndex, ZTIMER_tpfCallback pfCallback, void *pvParams, uint8 u8Flags)
{

    int n;
    ZTIMER_tsTimer *psTimer;

    DBG_vPrintf(TRACE_ZTIMER, "ZT: Open: ");

    /* Find a unused timer */
    for(n = 0; n < ZTIMER_sCommon.u8NumTimers; n++)
    {

        psTimer = &ZTIMER_sCommon.psTimers[n];

        if(psTimer->eState == E_ZTIMER_STATE_CLOSED)
        {
            psTimer->u8Flags             = u8Flags;
            psTimer->pvParameters        = pvParams;
            psTimer->pfCallback          = pfCallback;
            psTimer->u32Time             = 0;
            psTimer->eState              = E_ZTIMER_STATE_STOPPED;

            /* Return the index of the timer */
            *pu8TimerIndex = n;

            DBG_vPrintf(TRACE_ZTIMER, "Success (%d)\n", n);

            return E_ZTIMER_OK;

        }
    }

    DBG_vPrintf(TRACE_ZTIMER, "Failed\n");

    return E_ZTIMER_FAIL;

}


/****************************************************************************
 *
 * NAME: ZTIMER_eClose
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * ZTIMER_teStatus
 *
 ****************************************************************************/
PUBLIC ZTIMER_teStatus ZTIMER_eClose(uint8 u8TimerIndex)
{
    uint32 u32Store;
    DBG_vPrintf(TRACE_ZTIMER, "ZT: Close (%d): ", u8TimerIndex);

    if(u8TimerIndex > ZTIMER_sCommon.u8NumTimers)
    {
        DBG_vPrintf(TRACE_ZTIMER, "Failed\n");
        return E_ZTIMER_FAIL;
    }
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestricted(&u32Store);
#else
    MICRO_DISABLE_AND_SAVE_INTERRUPTS(u32Store);
#endif
    /* If this timer should prevent sleeping while running and the timer is currently running, decrease power manager activity count */
    if((ZTIMER_sCommon.psTimers[u8TimerIndex].u8Flags & ZTIMER_FLAG_PREVENT_SLEEP) && (ZTIMER_sCommon.psTimers[u8TimerIndex].eState == E_ZTIMER_STATE_RUNNING))
    {
        PWRM_eFinishActivity();
    }

    ZTIMER_sCommon.psTimers[u8TimerIndex].eState = E_ZTIMER_STATE_CLOSED;
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestore(&u32Store);
#else
    MICRO_RESTORE_INTERRUPTS(u32Store);
#endif

    DBG_vPrintf(TRACE_ZTIMER, "Success\n");
    
    return E_ZTIMER_OK;

}

/****************************************************************************
 *
 * NAME: ZTIMER_vStopAllTimers
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

PUBLIC void ZTIMER_vStopAllTimers(void)
{
    uint8_t n;
    ZTIMER_tsTimer *psTimer;
    
    for(n = 0; n < ZTIMER_sCommon.u8NumTimers; n++)
    {
        psTimer = &ZTIMER_sCommon.psTimers[n];

        /* If this timer is not opened and running, skip it */
        if(psTimer->eState != E_ZTIMER_STATE_RUNNING)
        {
            continue;
        }
        
        /* Stop this timer. */
        (void)ZTIMER_eStop(n);
    }
}


/****************************************************************************
 *
 * NAME: ZTIMER_eStart
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * ZTIMER_teStatus
 *
 ****************************************************************************/
PUBLIC ZTIMER_teStatus ZTIMER_eStart(uint8 u8TimerIndex, uint32 u32Time)
{
    uint32 u32Store;
    DBG_vPrintf(TRACE_ZTIMER, "ZT: Start (%d): ", u8TimerIndex);

    /* Check timer index is valid and the timer has previously been opened */
    if(u8TimerIndex > ZTIMER_sCommon.u8NumTimers || ZTIMER_sCommon.psTimers[u8TimerIndex].eState == E_ZTIMER_STATE_CLOSED || u32Time == 0)
    {
        DBG_vPrintf(TRACE_ZTIMER, "Failed\n");
        return E_ZTIMER_FAIL;
    }
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestricted(&u32Store);
#else
    MICRO_DISABLE_AND_SAVE_INTERRUPTS(u32Store);
#endif
    /* If this timer should prevent sleeping while running and the timer is not currently running, increase power manager activity count */
    if((ZTIMER_sCommon.psTimers[u8TimerIndex].u8Flags & ZTIMER_FLAG_PREVENT_SLEEP) && (ZTIMER_sCommon.psTimers[u8TimerIndex].eState != E_ZTIMER_STATE_RUNNING))
    {
        PWRM_eStartActivity();
    }
    /* Load the timer and start it */
    ZTIMER_sCommon.psTimers[u8TimerIndex].u32Time = u32Time;
    ZTIMER_sCommon.psTimers[u8TimerIndex].eState = E_ZTIMER_STATE_RUNNING;
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestore(&u32Store);
#else
    MICRO_RESTORE_INTERRUPTS(u32Store);
#endif
    DBG_vPrintf(TRACE_ZTIMER, "Success\n");

    return E_ZTIMER_OK;

}


/****************************************************************************
 *
 * NAME: ZTIMER_eStop
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * ZTIMER_teStatus
 *
 ****************************************************************************/
PUBLIC ZTIMER_teStatus ZTIMER_eStop(uint8 u8TimerIndex)
{
    uint32 u32Store;
    DBG_vPrintf(TRACE_ZTIMER, "ZT: Stop (%d): ", u8TimerIndex);

    /* Check timer index is valid and the timer has previously been opened */
    if(u8TimerIndex > ZTIMER_sCommon.u8NumTimers || ZTIMER_sCommon.psTimers[u8TimerIndex].eState == E_ZTIMER_STATE_CLOSED)
    {
        DBG_vPrintf(TRACE_ZTIMER, "Failed\n");
        return E_ZTIMER_FAIL;
    }
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestricted(&u32Store);
#else
    MICRO_DISABLE_AND_SAVE_INTERRUPTS(u32Store);
#endif
    /* If this timer should prevent sleeping while running and the timer is currently running, decrease power manager activity count */
    if((ZTIMER_sCommon.psTimers[u8TimerIndex].u8Flags & ZTIMER_FLAG_PREVENT_SLEEP) && (ZTIMER_sCommon.psTimers[u8TimerIndex].eState == E_ZTIMER_STATE_RUNNING))
    {
        PWRM_eFinishActivity();
    }

    /* Stop the timer */
    ZTIMER_sCommon.psTimers[u8TimerIndex].eState = E_ZTIMER_STATE_STOPPED;
#if ZIGBEE_USE_FRAMEWORK
    OSA_InterruptEnableRestore(&u32Store);
#else
    MICRO_RESTORE_INTERRUPTS(u32Store);
#endif
    DBG_vPrintf(TRACE_ZTIMER, "Success\n");

    return E_ZTIMER_OK;

}


/****************************************************************************
 *
 * NAME: ZTIMER_eGetState
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * ZTIMER_teState
 *
 ****************************************************************************/
PUBLIC ZTIMER_teState ZTIMER_eGetState(uint8 u8TimerIndex)
{
     return ZTIMER_sCommon.psTimers[u8TimerIndex].eState;
}


/****************************************************************************/
/***        Local Functions                                                 */
/****************************************************************************/

/****************************************************************************/
/*          END OF FILE                                                     */
/****************************************************************************/
