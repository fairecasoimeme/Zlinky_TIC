/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLo Sensor Demo
 *
 * COMPONENT:          app_sleep_handler.c
 *
 * DESCRIPTION:        ZLO Demo : Manages sleep configuration.
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>
#include "dbg.h"
#include "ZTimer.h"
#include "app_main.h"

#ifdef APP_LOW_POWER_API
#include "PWR_interface.h"
#else
#include "pwrm.h"
#endif

#include "pdum_gen.h"
#include "PDM.h"
#include "app_common.h"
#include "zcl_options.h"
#include "app_sleep_handler.h"
#include "app_zcl_tick_handler.h"
#include "app_zcl_sensor_task.h"
#include "App_Linky.h"
#include "app_events.h"

#include "app_sleep_handler.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_SLEEP_HANDLER
    #define TRACE_SLEEP_HANDLER   FALSE
#else
    #define TRACE_SLEEP_HANDLER   TRUE
#endif

#if ZLO_MAX_REPORT_INTERVAL == 0
    #define MAXIMUM_TIME_TO_SLEEP LIGHT_SENSOR_SAMPLING_TIME_IN_SECONDS + 1
#else
    //#define MAXIMUM_TIME_TO_SLEEP ZLO_MAX_REPORT_INTERVAL
	#define MAXIMUM_TIME_TO_SLEEP 6
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE uint8 u8NumberOfTimersTaskTimers(uint8 *pu8TimerMask);
PRIVATE void vScheduleSleep(bool_t bDeepSleep);
PRIVATE void vWakeCallBack(void);
PRIVATE void vStopNonSleepPreventingTimers(void);
PRIVATE void vStartNonSleepPreventingTimers(void);
PRIVATE uint8 u8NumberOfNonSleepPreventingTimers(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
#ifdef APP_LOW_POWER_API
extern uint8_t mLPMFlag;
#endif

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef APP_LOW_POWER_API
PUBLIC PWR_tsWakeTimerEvent sWake;
#else
PUBLIC pwrm_tsWakeTimerEvent sWake;
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME:        vAttemptToSleep
 *
 * DESCRIPTION: Checks to see if any software timers are running that may
 * prevent us from going to sleep. If there is none, if wake timer 0 is
 * running, schedule none deep sleep, if there is schedule deep sleep
 * which is checked if its enabled in vScheduleSleep.
 *
 ****************************************************************************/
PUBLIC void vAttemptToSleep(void)
{
    uint16 u16ActivityCount;
    uint8   u8ValMaskOfTimersTaskTimers;
    uint8   u8ValNumberOfTimersTaskTimers;
    uint8   u8ValNumberOfNonSleepPreventingTimers;

#ifdef APP_LOW_POWER_API
    u16ActivityCount = mLPMFlag;
#else
    u16ActivityCount = PWRM_u16GetActivityCount();
#endif
    u8ValNumberOfTimersTaskTimers = u8NumberOfTimersTaskTimers(&u8ValMaskOfTimersTaskTimers);
    u8ValNumberOfNonSleepPreventingTimers = u8NumberOfNonSleepPreventingTimers();

    //DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: Activity Count = %d", u16ActivityCount);
    //DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: Task Timers = %d", u8ValNumberOfTimersTaskTimers);

//    DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: vAttemptToSleep(), Act=%d, NotBlocking=%d, Blocking=%d (0x%02x), OSA_TimeGetMsec()=%d",
        //u16ActivityCount, u8ValNumberOfNonSleepPreventingTimers, u8ValNumberOfTimersTaskTimers, u8ValMaskOfTimersTaskTimers, OSA_TimeGetMsec());

    /* Only enter here if the activity count is equal to the number of non sleep preventing timers (in other words, the activity count
     * will become zero when we stop them) */



    if ((u16ActivityCount == u8ValNumberOfNonSleepPreventingTimers) &&
        (0 == u8ValNumberOfTimersTaskTimers) )
    {
        DBG_vPrintf(TRACE_SLEEP_HANDLER, ", SLEEP YES");
        /* Stop any background timers that are non sleep preventing*/
        vStopNonSleepPreventingTimers();

        /* Check if Wake timer 0 is running.*/
        if ((SYSCON->WKT_STAT&SYSCON_WKT_STAT_WKT0_RUNNING_MASK) == SYSCON_WKT_STAT_WKT0_RUNNING_MASK)
        {
            vScheduleSleep(FALSE);
            DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nvScheduleSleep(FALSE)");
        }
        else
        {
            vScheduleSleep(TRUE);
            DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nvScheduleSleep(TRUE)");
        }
    }
    else
    {
        //DBG_vPrintf(1, "\r\nu16ActivityCount = %d / u8ValNumberOfNonSleepPreventingTimers = %d ",u16ActivityCount,u8ValNumberOfNonSleepPreventingTimers);
    }
}


PUBLIC void ForceNoSleep(void)
{
	/*Start the APP_TickTimer to continue the ZCL tasks */
	vStartNonSleepPreventingTimers();

	APP_tsEvent sButtonEvent;
	sButtonEvent.eType = APP_E_EVENT_PERIODIC_REPORT;
	ZQ_bQueueSend(&APP_msgAppEvents, &sButtonEvent);
}
/****************************************************************************/
/***        Local Function                                                ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:        u8NumberOfTimersTaskTimers
 *
 * DESCRIPTION: Checks to see if any timers are running that shouldn't be
 * interrupted by sleeping.
 *
 ****************************************************************************/
PRIVATE uint8 u8NumberOfTimersTaskTimers(uint8 *pu8TimerMask)
{
    uint8 u8NumberOfRunningTimers = 0;
    *pu8TimerMask = 0;

    if (ZTIMER_eGetState(u8TimerButtonScan) == E_ZTIMER_STATE_RUNNING)
    {
//        DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: u8TimerButtonScan");
        u8NumberOfRunningTimers++;
        *pu8TimerMask |= 0x01;
    }

    if (ZTIMER_eGetState(u8TimerPoll) == E_ZTIMER_STATE_RUNNING)
    {
//        DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: u8TimerPoll");
        u8NumberOfRunningTimers++;
        *pu8TimerMask |= 0x02;
    }

    if (ZTIMER_eGetState(u8TimerBlink) == E_ZTIMER_STATE_RUNNING)
    {
//        DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: u8TimerBlink");
        u8NumberOfRunningTimers++;
        *pu8TimerMask |= 0x04;
    }
#if (defined APP_NTAG_ICODE)
    if (ZTIMER_eGetState(u8TimerNtag) == E_ZTIMER_STATE_RUNNING)
    {
//        DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: u8TimerNtag");
        u8NumberOfRunningTimers++;
        *pu8TimerMask |= 0x08;
    }
#endif
    /* lpsw8572: include timers for power on count and light sensor sample to not stay awake too long */
    if (ZTIMER_eGetState(u8TimerPowerOnCount) == E_ZTIMER_STATE_RUNNING)
    {
//        DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: u8TimerPowerOnCount");
        u8NumberOfRunningTimers++;
        *pu8TimerMask |= 0x10;
    }

    return u8NumberOfRunningTimers;
}

/****************************************************************************
 *
 * NAME:        vScheduleSleep
 *
 * DESCRIPTION: If we have deep sleep enabled and we attempting to deep sleep
 * then re-initialise the power manager for deep sleep
 *
 ****************************************************************************/
PRIVATE void vScheduleSleep(bool_t bDeepSleep)
{
#ifdef DEEP_SLEEP_ENABLE
    if (bDeepSleep)
    {
#ifdef APP_LOW_POWER_API
        (void) PWR_ChangeDeepSleepMode(PWR_E_SLEEP_OSCOFF_RAMOFF);
        PWR_Init();
        PWR_vForceRadioRetention(FALSE);
#else
        PWRM_vInit(E_AHI_SLEEP_OSCOFF_RAMOFF);
        PWRM_vForceRadioRetention(FALSE);
#endif
        DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: Deep Sleep");
    }
    else
    {
#ifdef APP_LOW_POWER_API
        PWR_eScheduleActivity(&sWake, (MAXIMUM_TIME_TO_SLEEP - u32GetNumberOfZCLTicksSinceReport())*APP_TICKS_PER_SECOND , vWakeCallBack);
#else
        PWRM_eScheduleActivity(&sWake, (MAXIMUM_TIME_TO_SLEEP - u32GetNumberOfZCLTicksSinceReport())*APP_TICKS_PER_SECOND , vWakeCallBack);
#endif
        DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: Osc on");
    }
#else
#ifdef CLD_OTA
    if(eOTA_GetState() == OTA_DL_PROGRESS)
#ifdef APP_LOW_POWER_API
        PWR_eScheduleActivity(&sWake, SENSOR_OTA_SLEEP_IN_SECONDS * APP_TICKS_PER_SECOND , vWakeCallBack);
#else
        PWRM_eScheduleActivity(&sWake, SENSOR_OTA_SLEEP_IN_SECONDS * APP_TICKS_PER_SECOND , vWakeCallBack);
#endif
    else
#endif
#ifdef APP_LOW_POWER_API
       // PWR_eScheduleActivity(&sWake, (MAXIMUM_TIME_TO_SLEEP - u32GetNumberOfZCLTicksSinceReport())*APP_TICKS_PER_SECOND , vWakeCallBack);
    	PWR_eScheduleActivity(&sWake, MAXIMUM_TIME_TO_SLEEP * APP_TICKS_PER_SECOND , vWakeCallBack);
        DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSPWR_eScheduleActivity %ld",MAXIMUM_TIME_TO_SLEEP * APP_TICKS_PER_SECOND);
#else
        PWRM_eScheduleActivity(&sWake, (MAXIMUM_TIME_TO_SLEEP - u32GetNumberOfZCLTicksSinceReport())*APP_TICKS_PER_SECOND , vWakeCallBack);
#endif
#endif
}

/****************************************************************************
 *
 * NAME:        vStopNonSleepPreventingTimers
 *
 * DESCRIPTION: The timers in this function should not stop us from sleep.
 * Stop the timers to reduce the activity count which will prevent sleep.
 *
 ****************************************************************************/
PRIVATE void vStopNonSleepPreventingTimers()
{
#ifndef DEEP_SLEEP_ENABLE
    if (ZTIMER_eGetState(u8TimerTick) != E_ZTIMER_STATE_STOPPED)
    {
        ZTIMER_eStop(u8TimerTick);
    }
    if (ZTIMER_eGetState(u8TimerLightSensorSample) != E_ZTIMER_STATE_STOPPED)
    {
        ZTIMER_eStop(u8TimerLightSensorSample);
    }
#endif
}

/****************************************************************************
 *
 * NAME:        vStartNonSleepPreventingTimers
 *
 * DESCRIPTION: Start the timers that wont stop us in vAttemptToSleep
 *
 ****************************************************************************/
PRIVATE void vStartNonSleepPreventingTimers(void)
{
#ifndef DEEP_SLEEP_ENABLE
    if (ZTIMER_eGetState(u8TimerTick) != E_ZTIMER_STATE_RUNNING)
        ZTIMER_eStart(u8TimerTick, ZCL_TICK_TIME);
#endif
    /* Don't start u8TimerLightSensorSample as it gets started elsewhere */
}
/****************************************************************************
 *
 * NAME:        u8NumberOfNonSleepPreventingTimers
 *
 * DESCRIPTION: Returns the number of timers that are running that we are
 * prepared to stop before going to sleep.
 *
 ****************************************************************************/
PRIVATE uint8 u8NumberOfNonSleepPreventingTimers(void)
{
    uint8 u8NumberOfRunningTimers = 0;

    if (ZTIMER_eGetState(u8TimerTick) == E_ZTIMER_STATE_RUNNING)
    {
        //DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: u8TimerTick");
        u8NumberOfRunningTimers++;
    }
    if (ZTIMER_eGetState(u8TimerLightSensorSample) == E_ZTIMER_STATE_RUNNING)
    {
        //DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: u8TimerLightSensorSample");
        u8NumberOfRunningTimers++;
    }

    return u8NumberOfRunningTimers;
}

/****************************************************************************
 *
 * NAME: vWakeCallBack
 *
 * DESCRIPTION:
 * Wake up call back called upon wake up by the schedule activity event.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vWakeCallBack(void)
{
    //DBG_vPrintf(TRACE_SLEEP_HANDLER, "\r\nSLEEP: vWakeCallBack\n");

    /*Start the APP_TickTimer to continue the ZCL tasks */
    vStartNonSleepPreventingTimers();

    /* Post a message to the stack so we aren't handling events
     * in interrupt context
     * MJL: From SysCtrl interrupt in 6x/7x version
     */
    APP_tsEvent sButtonEvent;
    sButtonEvent.eType = APP_E_EVENT_PERIODIC_REPORT;
    ZQ_bQueueSend(&APP_msgAppEvents, &sButtonEvent);
}


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
