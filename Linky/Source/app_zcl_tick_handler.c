/*****************************************************************************
 *
 * MODULE:          JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:       app_zcl_tick_handler.c
 *
 * DESCRIPTION:     ZLO Demo: Ticks the ZCL and handles ticks while asleep
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
#include "dbg.h"
#include "App_Linky.h"
#include "app_zcl_tick_handler.h"
#include "app_common.h"
#include "app_nwk_event_handler.h"
#include "app_main.h"

#ifdef CLD_OTA
#include "app_ota_client.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_ZCL_PROCESSING
    #define TRACE_ZCL_PROCESSING   TRUE
#else
    #define TRACE_ZCL_PROCESSING   FALSE
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

PRIVATE uint32 u32NumberOfZCLTicksSinceLastReport = 0;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:        vDecrementTickCount
 *
 * DESCRIPTION: Increments the count since last report or resets it if
 * it reaches the max report interval
 *
 ****************************************************************************/
PUBLIC void vDecrementTickCount(void)
{
    if (u32NumberOfZCLTicksSinceLastReport == ZLO_MAX_REPORT_INTERVAL)
    {
        u32NumberOfZCLTicksSinceLastReport = 0;
    }
    else
    {
        u32NumberOfZCLTicksSinceLastReport++;
    }
}

/****************************************************************************
 *
 * NAME:        u32GetNumberOfZCLTicksSinceReport
 *
 * DESCRIPTION: Gets the number of ticks since the last report
 *
 ****************************************************************************/
PUBLIC uint32 u32GetNumberOfZCLTicksSinceReport(void)
{
    return u32NumberOfZCLTicksSinceLastReport;
}

/****************************************************************************
 *
 * NAME:        vUpdateZCLTickSinceSleep
 *
 * DESCRIPTION: Is called when woken up and ticks the ZCL the amount of seconds
 * it was asleep for.
 *
 ****************************************************************************/
PUBLIC void vUpdateZCLTickSinceSleep(void)
{
    ZPS_tsAfEvent sStackEvent;
    tsZCL_CallBackEvent sCallBackEvent;
    sCallBackEvent.pZPSevent = &sStackEvent;
    uint32 u32SleepCount = ZLO_MAX_REPORT_INTERVAL/6;

#ifdef CLD_OTA
    if(eOTA_GetState() == OTA_DL_PROGRESS)
        u32SleepCount = SENSOR_OTA_SLEEP_IN_SECONDS;
#endif

    for (;u32NumberOfZCLTicksSinceLastReport < u32SleepCount;u32NumberOfZCLTicksSinceLastReport++)
    {
        sCallBackEvent.eEventType = E_ZCL_CBET_TIMER;
        vZCL_EventHandler(&sCallBackEvent);
    }

#ifdef CLD_OTA
     /* Update for 1 second (1000ms) */
     vRunAppOTAStateMachine(u32NumberOfZCLTicksSinceLastReport*1000);
     if((eOTA_GetState() == OTA_FIND_SERVER_WAIT) ||
             (eOTA_GetState() == OTA_IEEE_WAIT) ||
             (eOTA_GetState() == OTA_QUERYIMAGE_WAIT))
     {
         vStartPollTimer(POLL_TIME_FAST);
     }
#endif

    u32NumberOfZCLTicksSinceLastReport = 0;

}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
