/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#ifdef FSL_RTOS_FREE_RTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "portmacro.h"

#if DEBUG_STACK_DEPTH
/* For uxTaskGetStackHighWaterMark */
#include "task.h"
/* For DBG_vPrintf */
#include "dbg.h"
#endif
#endif

#include "EmbeddedTypes.h"
#include "ZQueue.h"
#include "ZTimer.h"
#include "zigbee_config.h"
#include "fsl_gpio.h"
#include "RNG_Interface.h"
#include "SecLib.h"
#include "MemManager.h"
#include "app_buttons.h"
#include "app_main.h"
#include "app_router_node.h"
#include "app_leds.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(LNT_MODE_APP) || defined(KPI_MODE_APP)
#define APP_NUM_STD_TMRS             (3)
#else
#define APP_NUM_STD_TMRS             (2)
#endif
#define APP_QUEUE_SIZE               (1)
#define APP_ZTIMER_STORAGE           ( APP_NUM_STD_TMRS )


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
uint8_t  u8TimerButtonScan;
uint8_t  u8LedTimer;

#ifdef LNT_MODE_APP
#include "bdb_api.h"

uint8 u8LntTimerTick;

void APP_cbLntTimerTick(void *pvParam)
{
    BDB_eNsStartNwkSteering();
}
#endif

/* queue handles */
tszQueue APP_msgAppEvents;
uint32_t u32Togglems;
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static ZTIMER_tsTimer asTimers[APP_ZTIMER_STORAGE + ZIGBEE_TIMER_STORAGE];
extern const uint8_t gUseRtos_c;




/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

extern void vAppMain(void);

#ifndef DUAL_MODE_APP
/****************************************************************************
 *
 * NAME: main_task
 *
 * DESCRIPTION:
 * Main  execution loop
 *
 * RETURNS:
 * Never
 *
 ****************************************************************************/
void main_task (uint32_t parameter)
{
#if defined(FSL_RTOS_FREE_RTOS) && DEBUG_STACK_DEPTH
	UBaseType_t uxHighWaterMark;
#endif

    /* e.g. osaEventFlags_t ev; */
    static uint8_t initialized = FALSE;

    if(!initialized)
    {
        /* place initialization code here... */
        initialized = TRUE;
        RNG_Init();
        SecLib_Init();
        MEM_Init();
        vAppMain();
#if defined(FSL_RTOS_FREE_RTOS) && DEBUG_STACK_DEPTH
        uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
        DBG_vPrintf(TRUE, "Stack High Watermark = %d B\r\n", uxHighWaterMark * sizeof(unsigned int));
#endif
    }

    while(1)
    {

         /* place event handler code here... */
        APP_vRunZigbee();
        ZTIMER_vTask();

        APP_taskRouter();


        if(!gUseRtos_c)
        {
            break;
        }
    }
}
#endif

/****************************************************************************
 *
 * NAME: APP_vInitResources
 *
 * DESCRIPTION:
 * Initialise resources (timers, queue's etc)
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void APP_vInitResources(void)
{
    /* Initialise the Z timer module */
    ZTIMER_eInit(asTimers, sizeof(asTimers) / sizeof(ZTIMER_tsTimer));

    /* Create Z timers */
    ZTIMER_eOpen(&u8TimerButtonScan,         APP_cbTimerButtonScan,  NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8LedTimer,           APP_cbTimerLed,         NULL, ZTIMER_FLAG_PREVENT_SLEEP);
#ifdef LNT_MODE_APP
    ZTIMER_eOpen(&u8LntTimerTick,       APP_cbLntTimerTick,		NULL, ZTIMER_FLAG_PREVENT_SLEEP);
#endif
    ZQ_vQueueCreate(&APP_msgAppEvents,        APP_QUEUE_SIZE,       sizeof(APP_tsEvent),         NULL);
}


/****************************************************************************
*
* NAME: APP_cbTimerLed
*
* DESCRIPTION:
* Timer callback to to toggle LEDs
*
* PARAMETER:
*
* RETURNS:
*
****************************************************************************/
void APP_cbTimerLed(void *pvParam)
{
	static bool_t bCurrentState = TRUE;
	APP_vSetLed(LED2, bCurrentState);

	if( ZPS_bGetPermitJoiningStatus()|| u32Togglems != 250)
	{
		ZTIMER_eStop(u8LedTimer);
		ZTIMER_eStart(u8LedTimer, ZTIMER_TIME_MSEC(u32Togglems));
	    bCurrentState = !bCurrentState;
	}
	else
	{
		APP_vSetLed(LED2, ON);
	}
}
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
