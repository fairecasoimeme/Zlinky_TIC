/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          app_zlo_sensor_node.c
 *
 * DESCRIPTION:        ZLO Demo : Stack <-> Light Sensor App Interaction
 *                     (Implementation)
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
 * Copyright NXP B.V. 2016-2019. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/


#include <jendefs.h>
#include <AppApi.h>
#include "pdum_apl.h"
#include "pdum_gen.h"
#include "PDM.h"
#include "dbg.h"
#include "zps_gen.h"
#include "rnd_pub.h"
#include "app_common.h"
#include "Groups.h"
#include "PDM_IDs.h"
#include "ZTimer.h"
#include "app.h"
#include "app_zlo_sensor_node.h"
#include "app_zcl_sensor_task.h"
#include "app_zbp_utilities.h"
#include "app_events.h"
#include "zcl_customcommand.h"
#include "app_main.h"
#include "app_linky_state_machine.h"
#include "app_reporting.h"
#if ((defined OM15081R1) || (defined OM15081R2))
    #ifdef OM15081R1
        #include "TSL2550.h"
    #endif
    #ifdef OM15081R2
        #include "TSL25721.h"
    #endif
#else
    #error Light Sensor driver not set!
#endif
#include <app_linky_buttons.h>
#include "app_power_on_counter.h"
#include "app_event_handler.h"
#include "app_nwk_event_handler.h"
#include "app_blink_led.h"
#include "bdb_api.h"

#include "fsl_os_abstraction.h"
#ifdef APP_NTAG_ICODE
#include "app_ntag_icode.h"
#include "nfc_nwk.h"
#endif
#ifdef CLD_OTA
    #include "app_ota_client.h"
#endif
#include "MicroSpecific.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_SENSOR_NODE
    #define TRACE_SENSOR_NODE   TRUE
#else
    #define TRACE_SENSOR_NODE   FALSE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void app_vStartNodeFactoryNew(void);
PRIVATE void app_vRestartNode (void);
PRIVATE uint8 app_u8GetSensorEndpoint( void);
PRIVATE void vAPP_InitialiseTask(void);
PRIVATE void vDeletePDMOnButtonPress(uint8 u8ButtonDIO);
PRIVATE void vAppHandleAfEvent( BDB_tsZpsAfEvent *psZpsAfEvent);
PRIVATE void vAppHandleZdoEvents( BDB_tsZpsAfEvent *psZpsAfEvent);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

PUBLIC tsDeviceDesc           sDeviceDesc;
PUBLIC bool_t bBDBJoinFailed = FALSE;
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
uint8 u8NoQueryCount = 0;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#ifdef K32WMCM_APP_BUILD
/* Must be called before zps_eAplAfInit() */
void APP_SetHighTxPowerMode();

/* Must be called after zps_eAplAfInit() */
void APP_SetMaxTxPower();

#undef HIGH_TX_PWR_LIMIT
#define HIGH_TX_PWR_LIMIT 15	/* dBm */
/* High Tx power */
void APP_SetHighTxPowerMode()
{
	if (CHIP_IS_HITXPOWER_CAPABLE())
		vMMAC_SetTxPowerMode(TRUE);
}

void APP_SetMaxTxPower()
{
	if (CHIP_IS_HITXPOWER_CAPABLE())
		ZPS_eMacPlmeSet(PHY_PIB_ATTR_TX_POWER, HIGH_TX_PWR_LIMIT);
}
#endif

/****************************************************************************
 *
 * NAME: APP_vInitialiseNode
 *
 * DESCRIPTION:
 * Initialises the application related functions
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vInitialiseNode(void)
{
    PDM_teStatus eStatusReportReload;
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() ENTER");


#ifdef CLD_OTA
    vLoadOTAPersistedData();
#endif

    vManagePowerOnCountLoadRecord();


    /*In case of a deep sleep device any button wake up would cause a PDM delete , only check for DIO8
     * pressed for deleting the context */
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> PMC->RESETCAUSE = %d", PMC->RESETCAUSE);


    /* Restore any report data that is previously saved to flash */
    eStatusReportReload = eRestoreReports();


    uint16 u16ByteRead;
    PDM_eReadDataFromRecord(PDM_ID_APP_SENSOR,
                           &sDeviceDesc,
                           sizeof(tsDeviceDesc),
                           &u16ByteRead);

    APP_vRadioTempUpdate(TRUE);

#ifdef K32WMCM_APP_BUILD
    APP_SetHighTxPowerMode();
#endif

    /* Initialize ZBPro stack */
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> ZPS_eAplAfInit()");
    ZPS_eAplAfInit();

#ifdef K32WMCM_APP_BUILD
    APP_SetMaxTxPower();
#endif

    APP_ZCL_vInitialise();

    /* Set End Device TimeOut */
    ZPS_bAplAfSetEndDeviceTimeout( ZED_TIMEOUT_2_MIN);

    /*Load the reports from the PDM or the default ones depending on the PDM load record status*/
    if(eStatusReportReload !=PDM_E_STATUS_OK )
    {
        /*Load Defaults if the data was not correct*/
    	DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: vLoadDefaultConfigForReportable");
        vLoadDefaultConfigForReportable();
    }
    /*Make the reportable attributes */
    vMakeSupportedAttributesReportable();

    /* If the device state has been restored from flash, re-start the stack
     * and set the application running again.
     */
    sBDB.sAttrib.u32bdbPrimaryChannelSet = BDB_PRIMARY_CHANNEL_SET;
    sBDB.sAttrib.u32bdbSecondaryChannelSet = BDB_SECONDARY_CHANNEL_SET;
    BDB_tsInitArgs sInitArgs;
    sInitArgs.hBdbEventsMsgQ = &APP_msgBdbEvents;

    BDB_vInit(&sInitArgs);

    if (APP_bNodeIsInRunningState())
    {
        app_vRestartNode();
        sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
    }
    else
    {
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode(), Factory New Start");
        app_vStartNodeFactoryNew();
        sBDB.sAttrib.bbdbNodeIsOnANetwork = FALSE;
    }

    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> APP_bButtonInitialise()");
    APP_bButtonInitialise();
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> vDeletePDMOnButtonPress()");
    vDeletePDMOnButtonPress(APP_BOARD_SW0_PIN);

    #ifdef PDM_EEPROM
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> vDisplayPDMUsage()");
        vDisplayPDMUsage();
    #endif

    GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 1); // Off

    /* Always initialise the light sensor and the RGB Led
     * the iic interface runs slower (100kHz) than that used
     * by the RGB driver (400KHz)
     * */
#ifdef OM15081R1
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> bTSL2550_Init()");
    bool_t bStatus= bTSL2550_Init();
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> bTSL2550_Init() = %d", bStatus);
#endif
#ifdef OM15081R2
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> bTSL25721_Init()");
    bool_t bStatus= bTSL25721_Init();
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() -> bTSL25721_Init() = %d", bStatus);
#endif

    vAPP_LinkySensorSample();
    vAPP_InitialiseTask();
    

    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vInitialiseNode() EXIT");
}

/****************************************************************************
 *
 * NAME: vAPP_InitialiseTask
 *
 * DESCRIPTION:
 * This is the main App Initialise.
 * Task that checks the power status and starts tasks based on those results
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vAPP_InitialiseTask( void)
{
    vManagePowerOnCountInit();

    if (APP_bNodeIsInRunningState())
    {
//        vActionOnButtonActivationAfterDeepSleep();
    }
    else
    {
        /* We are factory new so start the blink timers*/
        vStartBlinkTimer(APP_JOINING_BLINK_TIME);
    }
}

/****************************************************************************
 *
 * NAME: APP_vBdbCallback
 *
 * DESCRIPTION:
 * Callbak from the BDB
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vBdbCallback(BDB_tsBdbEvent *psBdbEvent)
{
#if 0 //LJM
    vAHI_WatchdogRestart(); // JV - Assuming bdb_taskBDB() context might take longer; especially during initializaiton. ToDo - discuss with Richard towards removal from here
#endif
   // DBG_vPrintf(TRACE_SENSOR_NODE, "\r\npsBdbEvent->eEventType %d",psBdbEvent->eEventType);
    switch(psBdbEvent->eEventType)
    {
    case BDB_EVENT_NONE:
        break;

    case BDB_EVENT_ZPSAF:                // Use with BDB_tsZpsAfEvent
        vAppHandleAfEvent(&psBdbEvent->uEventData.sZpsAfEvent);
        break;

    case BDB_EVENT_INIT_SUCCESS:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(INIT_SUCCESS)");
        break;

    case BDB_EVENT_FAILURE_RECOVERY_FOR_REJOIN: //Recovery on rejoin failure
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FAILURE_RECOVERY_FOR_REJOIN)");
        vHandleFailedRejoin();
        break;

    case BDB_EVENT_REJOIN_FAILURE: // only for ZED
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(REJOIN_FAILURE)");
        vHandleFailedToJoin();
        break;

    case BDB_EVENT_REJOIN_SUCCESS: // only for ZED
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(REJOIN_SUCCESS)");
        bBDBJoinFailed = FALSE;
        vStartPollTimer(POLL_TIME_FAST);
        vHandleNetworkJoinEndDevice();
        vStartJoinIdle(5);
        break;

    case BDB_EVENT_NWK_STEERING_SUCCESS:
        // go to running state
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(NWK_STEERING_SUCCESS)");
        bBDBJoinFailed = FALSE;
        vHandleNetworkJoinAndRejoin();
        vStartJoinIdle(15);
        break;

    case BDB_EVENT_NO_NETWORK:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(NO_NETWORK)");
        vHandleFailedToJoin();
        break;

    case BDB_EVENT_APP_START_POLLING:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(APP_START_POLLING)");
        /* Start fast polling */
        vStartPollTimer(POLL_TIME_FAST);
        break;

    case BDB_EVENT_FB_HANDLE_SIMPLE_DESC_RESP_OF_TARGET:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_HANDLE_SIMPLE_DESC_RESP_OF_TARGET)");
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: Simple descriptor %d %d %04x %04x %d",psBdbEvent->uEventData.psFindAndBindEvent->u8TargetEp,
                psBdbEvent->uEventData.psFindAndBindEvent->u16TargetAddress,
                psBdbEvent->uEventData.psFindAndBindEvent->u16ProfileId,
                psBdbEvent->uEventData.psFindAndBindEvent->u16DeviceId,
                psBdbEvent->uEventData.psFindAndBindEvent->u8DeviceVersion);
        break;

    case BDB_EVENT_FB_CHECK_BEFORE_BINDING_CLUSTER_FOR_TARGET:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_CHECK_BEFORE_BINDING_CLUSTER_FOR_TARGET), u16ClusterId=0x%x", psBdbEvent->uEventData.psFindAndBindEvent->uEvent.u16ClusterId);
        break;

    case BDB_EVENT_FB_CLUSTER_BIND_CREATED_FOR_TARGET:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_CLUSTER_BIND_CREATED_FOR_TARGET), u16ClusterId=0x%x", psBdbEvent->uEventData.psFindAndBindEvent->uEvent.u16ClusterId);
        break;

    case BDB_EVENT_FB_BIND_CREATED_FOR_TARGET:
        {
            DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_BIND_CREATED_FOR_TARGET), u8TargetEp=%d", psBdbEvent->uEventData.psFindAndBindEvent->u8TargetEp);
            u8NoQueryCount = 0;

            uint8 u8Seq;
            tsZCL_Address sAddress;
            tsCLD_Identify_IdentifyRequestPayload sPayload;

            sPayload.u16IdentifyTime = 0;
            sAddress.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
            sAddress.uAddress.u16DestinationAddress = psBdbEvent->uEventData.psFindAndBindEvent->u16TargetAddress;

            eCLD_IdentifyCommandIdentifyRequestSend(
                                    psBdbEvent->uEventData.psFindAndBindEvent->u8InitiatorEp,
                                    psBdbEvent->uEventData.psFindAndBindEvent->u8TargetEp,
                                    &sAddress,
                                    &u8Seq,
                                    &sPayload);
        }
        break;

    case BDB_EVENT_FB_GROUP_ADDED_TO_TARGET:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_GROUP_ADDED_TO_TARGET)");
        break;

    case BDB_EVENT_FB_ERR_BINDING_TABLE_FULL:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_ERR_BINDING_TABLE_FULL)");
        break;

    case BDB_EVENT_FB_ERR_BINDING_FAILED:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_ERR_BINDING_FAILED)");
        break;

    case BDB_EVENT_FB_ERR_GROUPING_FAILED:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_ERR_GROUPING_FAILED)");
        break;

    case BDB_EVENT_FB_NO_QUERY_RESPONSE:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_NO_QUERY_RESPONSE)");
        //Example to stop further query repeating
        if(u8NoQueryCount >= 2)
        {
            u8NoQueryCount = 0;
            BDB_vFbExitAsInitiator();
        }
        else
        {
            u8NoQueryCount++;
        }
        break;

    case BDB_EVENT_FB_TIMEOUT:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(FB_TIMEOUT)");
        break;

    default:
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vBdbCallback(default=%d)", psBdbEvent->eEventType);
        break;

    }
}

/****************************************************************************
 *
 * NAME: APP_ZLO_Sensor_Task
 *
 * DESCRIPTION:
 * Checks to see what event has triggered the task to start and calls the
 * appropriate function. This handles App events, Stack events, timer activations
 * and manual activations.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_taskSensor(void)
{
    APP_tsEvent sAppEvent;
    sAppEvent.eType = APP_E_EVENT_NONE;
    /*Collect the application events*/
    if (ZQ_bQueueReceive(&APP_msgAppEvents, &sAppEvent) == TRUE)
    {
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_taskSensor(), AppEventType=%d", sAppEvent.eType);
#if ((defined APP_NTAG_ICODE) && (APP_BUTTONS_NFC_FD != (0xff)))
        /* Is this a button event on field detect ? */
        if ( ( sAppEvent.eType == APP_E_EVENT_BUTTON_DOWN || sAppEvent.eType == APP_E_EVENT_BUTTON_UP )
               && ( sAppEvent.uEvent.sButton.u8Button == APP_E_BUTTONS_NFC_FD) )
        {
            /* Always pass this on for processing */
            vAppHandleAppEvent(sAppEvent);
        }
        /* Other event (handle as normal) ? */
        else
#endif
        {
            if(bBDBJoinFailed)
            {
                vStartBlinkTimer(APP_JOINING_BLINK_TIME);
                if(APP_bNodeIsInRunningState())
                {
                    // TODO kick BDB for rejoin
                	if (sAppEvent.eType==APP_E_EVENT_BUTTON_LONGPRESS)
                	{
                		sBDB.sAttrib.bbdbNodeIsOnANetwork = FALSE;
						if (ZPS_E_SUCCESS !=  ZPS_eAplZdoLeaveNetwork(0, FALSE,FALSE))
						{
							/* Leave failed, probably lost parent, so just reset everything */
							DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: vDeletePDMOnButtonPress() -> APP_vFactoryResetRecords()");
							APP_vFactoryResetRecords();
							MICRO_DISABLE_INTERRUPTS();
							RESET_SystemReset();
						}

                	}else{
						DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_taskSensor() -> BDB_vStart()");
						sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
						BDB_vStart();
                	}
                }
                else
                {
                    //Retrigger the network steering as sensor is not part of a network
                    vAppHandleStartup();
                }
            }
            else
            {
                vAppHandleAppEvent(sAppEvent);
            }
        }
    }
}

/****************************************************************************
 *
 * NAME: vAppHandleAfEvent
 *
 * DESCRIPTION:
 * Application handler for stack events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vAppHandleAfEvent( BDB_tsZpsAfEvent *psZpsAfEvent)
{

#ifdef CLD_OTA
    if (psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_DATA_INDICATION)
    {
        if ((psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.eStatus == ZPS_E_SUCCESS) &&
                (psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.u8DstEndpoint == 0))
        {
            // Data Ind for ZDp Ep
            if (ZPS_ZDP_MATCH_DESC_RSP_CLUSTER_ID == psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.u16ClusterId)
            {
                DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: ZPS_ZDP_MATCH_DESC_RSP_CLUSTER_ID");
                vHandleMatchDescriptor(&psZpsAfEvent->sStackEvent);
                if(APP_bPersistantPolling != TRUE)
                {
                    ZTIMER_eStop(u8TimerPoll);
                }
            } else if (ZPS_ZDP_IEEE_ADDR_RSP_CLUSTER_ID == psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.u16ClusterId) {
                vHandleIeeeAddressRsp(&psZpsAfEvent->sStackEvent);
                if(APP_bPersistantPolling != TRUE)
                {
                    ZTIMER_eStop(u8TimerPoll);
                }
            }
        }
    }
#endif
    if (psZpsAfEvent->u8EndPoint == app_u8GetSensorEndpoint())
    {
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: vAppHandleAfEvent(%d) -> APP_ZCL_vEventHandler(), OSA_TimeGetMsec()=%d", psZpsAfEvent->sStackEvent.eType, OSA_TimeGetMsec());
        APP_ZCL_vEventHandler( &psZpsAfEvent->sStackEvent);
    }
    else if (psZpsAfEvent->u8EndPoint == LINKY_ZDO_ENDPOINT)
    {
        DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: vAppHandleAfEvent(%d) -> vAppHandleZdoEvents(), OSA_TimeGetMsec()=%d", psZpsAfEvent->sStackEvent.eType, OSA_TimeGetMsec());
        vAppHandleZdoEvents( psZpsAfEvent);
    }

    /* Ensure Freeing of Apdus */
    if (psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_DATA_INDICATION)
    {
        PDUM_eAPduFreeAPduInstance(psZpsAfEvent->sStackEvent.uEvent.sApsDataIndEvent.hAPduInst);
    }
    else if ( psZpsAfEvent->sStackEvent.eType == ZPS_EVENT_APS_INTERPAN_DATA_INDICATION )
    {
        PDUM_eAPduFreeAPduInstance(psZpsAfEvent->sStackEvent.uEvent.sApsInterPanDataIndEvent.hAPduInst);
    }

}

/****************************************************************************
 *
 * NAME: vDeletePDMOnButtonPress
 *
 * DESCRIPTION:
 * PDM context clearing on button press
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vDeletePDMOnButtonPress(uint8 u8ButtonDIO)
{
    bool_t bDeleteRecords = FALSE;

    uint32 u32Buttons = APP_u32GetSwitchIOState() & (1 << u8ButtonDIO);

    if (u32Buttons == 0)
    {
        bDeleteRecords = TRUE;
    }
    else
    {
        bDeleteRecords = FALSE;
    }
    /* If required, at this point delete the network context from flash, perhaps upon some condition
     * For example, check if a button is being held down at reset, and if so request the Persistent
     * Data Manager to delete all its records:
     * e.g. bDeleteRecords = vCheckButtons();
     * Alternatively, always call PDM_vDelete() if context saving is not required.
     */
    if(bDeleteRecords)
    {
        sBDB.sAttrib.bbdbNodeIsOnANetwork = FALSE;
        if (ZPS_E_SUCCESS !=  ZPS_eAplZdoLeaveNetwork(0, FALSE,FALSE))
        {
            /* Leave failed, probably lost parent, so just reset everything */
            DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: vDeletePDMOnButtonPress() -> APP_vFactoryResetRecords()");
            APP_vFactoryResetRecords();
            MICRO_DISABLE_INTERRUPTS();
            RESET_SystemReset();
        }
    }
}


/****************************************************************************
 *
 * NAME: vAppHandleZdoEvents
 *
 * DESCRIPTION:
 * This is the main state machine which decides whether to call up the startup
 * or running function. This depends on whether we are in the network on not.
 *
 * PARAMETERS:
 * ZPS_tsAfEvent sAppStackEvent Stack event information.
 *
 ****************************************************************************/
PRIVATE void vAppHandleZdoEvents( BDB_tsZpsAfEvent *psZpsAfEvent)
{
#ifdef DEBUG_SENSOR_NODE
    //vDisplayNWKTransmitTable();
#endif

    /* Handle events depending on node state */
    switch (sDeviceDesc.eNodeState)
    {

    case E_STARTUP:
        vAppHandleStartup();
        break;

    case E_RUNNING:
        vAppHandleRunning( &(psZpsAfEvent->sStackEvent) );
        break;

    case E_JOINING_NETWORK:
        break;

    default:
        break;

    }

}

/****************************************************************************
 *
 * NAME: app_vRestartNode
 *
 * DESCRIPTION:
 * Start the Restart the ZigBee Stack after a context restore from
 * the EEPROM/Flash
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void app_vRestartNode (void)
{

    /* The node is in running state indicates that
     * the EZ Mode state is as E_EZ_DEVICE_IN_NETWORK*/
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: app_vRestartNode(), not factory new");

    ZPS_vSaveAllZpsRecords();
}


/****************************************************************************
 *
 * NAME: app_vStartNodeFactoryNew
 *
 * DESCRIPTION:
 * Start the ZigBee Stack for the first ever Time.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void app_vStartNodeFactoryNew(void)
{
    sBDB.sAttrib.u32bdbPrimaryChannelSet = BDB_PRIMARY_CHANNEL_SET;
    sBDB.sAttrib.u32bdbSecondaryChannelSet = BDB_SECONDARY_CHANNEL_SET;
    // BDB_eNsStartNwkSteering();  done later on  only if no OOB commissionning ????

    sDeviceDesc.eNodeState = E_JOINING_NETWORK;
    /* Stay awake for joining */
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: app_vStartNodeFactoryNew()");
}

/****************************************************************************
 *
 * NAME: app_u8GetSensorEndpoint
 *
 * DESCRIPTION:
 * Return the application endpoint
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PRIVATE uint8 app_u8GetSensorEndpoint( void)
{
    return LINKY_SENSOR_ENDPOINT;

}

/****************************************************************************
 *
 * NAME: APP_vFactoryResetRecords
 *
 * DESCRIPTION: reset application and stack to factory new state
 *              preserving the outgoing nwk frame counter
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vFactoryResetRecords(void)
{
    sDeviceDesc.eNodeState = E_STARTUP;

    /* clear out the stack */
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_vFactoryResetRecords(), Call clear stack");
    ZPS_vDefaultStack();
    ZPS_eAplAibSetApsUseExtendedPanId(0);
    ZPS_vSetKeys();

#ifdef CLD_OTA
    sDeviceDesc.bValid = FALSE;
    sDeviceDesc.u64IeeeAddrOfServer = 0;
    sDeviceDesc.u16NwkAddrOfServer = 0xffff;
    sDeviceDesc.u8OTAserverEP = 0xff;
#endif

    /* save everything */
    PDM_eSaveRecordData(PDM_ID_APP_SENSOR, &sDeviceDesc, sizeof(tsDeviceDesc));
    ZPS_vSaveAllZpsRecords();

}

PUBLIC bool_t APP_bNodeIsInRunningState(void)
{
    DBG_vPrintf(TRACE_SENSOR_NODE, "\r\nNODE: APP_bNodeIsInRunningState()=%d, eNodeState=%d", ((sDeviceDesc.eNodeState == E_RUNNING) ? TRUE : FALSE), sDeviceDesc.eNodeState);
    return (sDeviceDesc.eNodeState == E_RUNNING) ? TRUE : FALSE;
}

#ifdef CLD_OTA
/****************************************************************************
 *
 * NAME: eGetNodeState
 *
 * DESCRIPTION:
 * Returns the node state
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC teNodeState eGetNodeState(void)
{
    return sDeviceDesc.eNodeState;
}
/****************************************************************************
 *
 * NAME: sGetOTACallBackPersistdata
 *
 * DESCRIPTION:
 * returns persisted data
 *
 * RETURNS:
 * tsOTA_PersistedData
 *
 ****************************************************************************/

PUBLIC tsOTA_PersistedData sGetOTACallBackPersistdata(void)
{
    return sSensor.sCLD_OTA_CustomDataStruct.sOTACallBackMessage.sPersistedData;
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
