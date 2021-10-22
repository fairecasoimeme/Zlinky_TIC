/*****************************************************************************
 *
 * MODULE:          JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:       app_nwk_event_handler.c
 *
 * DESCRIPTION:     ZLO Demo: Handles all network events like network join/leave
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
#include "app_events.h"
#include "ZTimer.h"
#include "app_main.h"
#include "App_Linky.h"
#include "app_common.h"
#include "app_zbp_utilities.h"
#include "app_nwk_event_handler.h"
#include "app_blink_led.h"
#include "PDM.h"
#include "pdum_gen.h"
#include "PDM_IDs.h"
#include "app_zlo_sensor_node.h"
#include "app_event_handler.h"
#include "MicroSpecific.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_NWK_EVENT_HANDLER
    #define TRACE_NWK_EVENT_HANDLER   TRUE
#else
    #define TRACE_NWK_EVENT_HANDLER  FALSE
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
PUBLIC uint32 u32PollTime = 0;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: vStartPollTimer
 *
 * DESCRIPTION:
 * Function to start polling.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vStartPollTimer(uint32 u32PollInterval)
{
    u32PollTime = u32PollInterval;
    DBG_vPrintf(TRACE_NWK_EVENT_HANDLER, "\nAPP Starting poll timer with interval %d", u32PollInterval);
    ZTIMER_eStart(u8TimerPoll, u32PollTime);
}

/****************************************************************************
 *
 * NAME: APP_cbTimerPoll
 *
 * DESCRIPTION:
 * CallBack function for timed polling.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_cbTimerPoll( void * pvParam)
{
    ZPS_eAplZdoPoll();
    DBG_vPrintf(TRACE_NWK_EVENT_HANDLER, "\nAPP Poll Handler: Poll Sent, new poll time %d", u32PollTime);
    ZTIMER_eStart(u8TimerPoll, u32PollTime);
}

/****************************************************************************
 *
 * NAME: vHandlePollResponse
 *
 * DESCRIPTION:
 * Processes the poll response, Poll again if we have no ACK to force the Stack
 * to handle the failed poll count or poll to get the data.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vHandlePollResponse(ZPS_tsAfEvent* psStackEvent)
{
    DBG_vPrintf(TRACE_NWK_EVENT_HANDLER, "\nAPP NWK Event Handler: Poll Response");
    switch (psStackEvent->uEvent.sNwkPollConfirmEvent.u8Status)
    {
        case MAC_ENUM_SUCCESS:
        case MAC_ENUM_NO_ACK:
            ZPS_eAplZdoPoll();
            break;

        case MAC_ENUM_NO_DATA:
        default:
            break;
    }
}

/****************************************************************************
 *
 * NAME: vHandleFailedToJoin
 *
 * DESCRIPTION:
 * If we have failed to join/rejoin the network stop the blink timer to
 * go back to sleep
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vHandleFailedToJoin(void)
{
    DBG_vPrintf(TRACE_NWK_EVENT_HANDLER, "\nAPP NWK Event Handler: Failed Rejoin");
    /* In case Find And Bind or Keep alive in progress stop it and make sure we go to sleep */
    vEventStopFindAndBind();
    APP_bPersistantPolling &= FALSE;
    ZTIMER_eStop(u8TimerLightSensorSample);
    bBDBJoinFailed = TRUE;
}


/****************************************************************************
 *
 * NAME: vHandleFailedRejoin
 *
 * DESCRIPTION:
 * If we have failed to rejoin the network we start the blink timer.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vHandleFailedRejoin(void)
{
    /* Start Blink Timer to avoid sleeping */
    if(ZTIMER_eGetState(u8TimerBlink) != E_ZTIMER_STATE_RUNNING)
        vStartBlinkTimer(APP_JOINING_BLINK_TIME);
}

/****************************************************************************
 *
 * NAME: vHandleNetworkJoinEndDevice
 *
 * DESCRIPTION:
 * If we have joined a new network or rejoined, stop the timer tell the PIR
 * event handler.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vHandleNetworkJoinEndDevice(void)
{
    DBG_vPrintf(TRACE_NWK_EVENT_HANDLER, "\n\rAPP NWK Event Handler: Network Join End Device");

    ZPS_eAplAibSetApsUseExtendedPanId( ZPS_u64NwkNibGetEpid(ZPS_pvAplZdoGetNwkHandle()) );

    /* Don't turn the timers off if we're in persistent polling mode */
    /*if(APP_bPersistantPolling != TRUE)
    {
        ZTIMER_eStop(u8TimerPoll);
        vStopBlinkTimer();
    }*/

#if (defined APP_NTAG_ICODE)
    sDeviceDesc.eNodeState = E_RUNNING;
    PDM_eSaveRecordData(PDM_ID_APP_SENSOR,
            &sDeviceDesc,
            sizeof(tsDeviceDesc));
    ZPS_vSaveAllZpsRecords();
#endif
}

/****************************************************************************
 *
 * NAME: vHandleNetworkLeave
 *
 * DESCRIPTION:
 * We have left the network so restart as factory new (not sure why we don't
 * restart the joining rather than restarting the whole device).
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vHandleNetworkLeave(ZPS_tsAfEvent* psStackEvent)
{
    DBG_vPrintf(TRACE_NWK_EVENT_HANDLER, "\nAPP NWK Event Handler: Network Leave");
    if( psStackEvent->uEvent.sNwkLeaveIndicationEvent.u64ExtAddr == 0 &&
            (psStackEvent->uEvent.sNwkLeaveIndicationEvent.u8Rejoin == FALSE))
    {
        DBG_vPrintf(TRACE_NWK_EVENT_HANDLER, "\nAPP NWK Event Handler: ZDO Leave" );
        APP_vFactoryResetRecords();
        MICRO_DISABLE_INTERRUPTS();
        RESET_SystemReset();
    }
}

/****************************************************************************
 *
 * NAME: vHandleNetworkJoinAndRejoin
 *
 * DESCRIPTION:
 * Checks to see if we have joined a network. If we have, stop all the timers,
 * save the network state into PDM and attempt to sleep.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vHandleNetworkJoinAndRejoin(void)
{
    DBG_vPrintf(1, "\nDEVICE_IN_NETWORK");

    //ZTIMER_eStop(u8TimerPoll);
    vStopBlinkTimer();
    sDeviceDesc.eNodeState = E_RUNNING;

    PDM_eSaveRecordData(PDM_ID_APP_SENSOR,
                            &sDeviceDesc,
                            sizeof(tsDeviceDesc));
    ZPS_vSaveAllZpsRecords();



}

/****************************************************************************
 *
 * NAME: vHandleNetworkLeaveConfirm
 *
 * DESCRIPTION:
 * We have left the network so restart as factory new (not sure why we don't
 * restart the joining rather than restarting the whole device).
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vHandleNetworkLeaveConfirm(ZPS_tsAfEvent* psStackEvent)
{
    DBG_vPrintf(TRACE_NWK_EVENT_HANDLER, "\nAPP NWK Event Handler: Network Leave Confirm Addr %016llx",
            psStackEvent->uEvent.sNwkLeaveConfirmEvent.u64ExtAddr);
    if (psStackEvent->uEvent.sNwkLeaveConfirmEvent.u64ExtAddr == 0UL)
    {
        APP_vFactoryResetRecords();
        MICRO_DISABLE_INTERRUPTS();
        RESET_SystemReset();
    }
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
