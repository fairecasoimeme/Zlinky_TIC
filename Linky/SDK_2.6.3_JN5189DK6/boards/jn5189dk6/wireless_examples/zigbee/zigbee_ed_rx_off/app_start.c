/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "EmbeddedTypes.h"
#include "PDM.h"
#include "app_end_device_node.h"
#include "app_main.h"
#include "bdb_api.h"
#include "zigbee_config.h"
#include "app_ota_client.h"
#include "dbg.h"
#include "RNG_Interface.h"
#include "SecLib.h"
#include "MemManager.h"
#include "app_zcl_task.h"
#include "app_buttons.h"
#include "app_leds.h"
#include "PWR_Interface.h"
#include "pwrm.h"
#include "app_reporting.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef TRACE_APP
    #define TRACE_APP   TRUE
#endif

#define HALT_ON_EXCEPTION   FALSE

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

static void APP_vInitialise(bool_t bColdStart);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

extern void *_stack_low_water_mark;
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/**
 * Power manager Callback.
 * Called just before the device is put to sleep
 */

static void vAppPreSleep(void);
/**
 * Power manager Callback.
 * Called just after the device wakes up from sleep
 */
static void vAppWakeup(void);
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
extern void OSA_TimeInit(void);
extern uint8* ZPS_pu8AplZdoGetVsOUI(void);
/****************************************************************************
 *
 * NAME: vAppMain
 *
 * DESCRIPTION:
 * Entry point for application from a cold start.
 *
 * RETURNS:
 * Never returns.
 *
 ****************************************************************************/
void vAppMain(bool_t bColdStart)
{
    APP_vInitResources();
    APP_vInitZigbeeResources();
    APP_vInitialise(bColdStart);
    if(bColdStart)
    {
        BDB_vStart();
    }
    else
    {
        BDB_vRestart();
    }

}

/****************************************************************************
 *
 * NAME: vAppRegisterPWRMCallbacks
 *
 * DESCRIPTION:
 * Power manager callback.
 * Called to allow the application to register
 * sleep and wake callbacks.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vAppRegisterPWRCallbacks(void)
{
    PWR_RegisterLowPowerEnterCallback(vAppPreSleep);
    PWR_RegisterLowPowerExitCallback(vAppWakeup);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: vAppPreSleep
 *
 * DESCRIPTION:
 *
 * PreSleep call back by the power manager before the controller put into sleep.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

void vAppPreSleep(void)
{
    DBG_vPrintf(TRACE_APP, "sleeping \n");
    vSetReportDataForMinRetention();
    /* If the power mode is with RAM held do the following
     * else not required as the entry point will init everything*/
     vSetOTAPersistedDatForMinRetention();
     /* sleep memory held */
     vAppApiSaveMacSettings();
     /* Disable debug */
     DbgConsole_Deinit();

}

/****************************************************************************
 *
 * NAME: Wakeup
 *
 * DESCRIPTION:
 *
 * Wakeup call back by the power manager after the controller wakes up from sleep.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

void vAppWakeup(void)
{
    /* If the power status is OK and RAM held while sleeping
     * restore the MAC settings
     * */

    if( (PMC->RESETCAUSE & ( PMC_RESETCAUSE_WAKEUPIORESET_MASK |
                     PMC_RESETCAUSE_WAKEUPPWDNRESET_MASK ) )  )
    {

        vAppApiRestoreMacSettings();
        RNG_Init();
        SecLib_Init();
        MEM_Init();
        vAppMain(FALSE);
        APP_vAlignStatesAfterSleep();
    }
    DBG_vPrintf(TRACE_APP, "woken up \n");
}

/****************************************************************************
 *
 * NAME: APP_vInitialise
 *
 * DESCRIPTION:
 * Initialises Zigbee stack, hardware and application.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void APP_vInitialise(bool_t bColdStart)
{
    if(bColdStart)
    {
        PWR_ChangeDeepSleepMode((uint8_t)E_AHI_SLEEP_OSCON_RAMON);
        PWR_Init();
        PWR_vForceRadioRetention(TRUE);
        /* Initialise the Persistent Data Manager */
        PDM_eInitialise(1200, 63, NULL);
    }
    /* Initialise application */
    APP_vInitialiseEndDevice(bColdStart);
}


/****************************************************************************
 *
 * NAME: vWakeCallBack
 *
 * DESCRIPTION:
 * callback when wake timer expires.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vWakeCallBack(void)
{
    APP_ZCL_vStartTimers();
    if(ZTIMER_eStart(u8TimerPoll, POLL_TIME_FAST) != E_ZTIMER_OK)
    {
        DBG_vPrintf(TRACE_APP, "\r\nAPP: Failed to start poll");
    }
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
