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
#include "dbg.h"
#include "PDM_IDs.h"
#include "ZTimer.h"
#include "pdum_gen.h"
#include "zcl.h"
#include "zcl_options.h"
#include "app_zcl_task.h"
#include "app.h"
#include "base_device.h"
#include "app_common.h"
#include "app_main.h"
#include "bdb_api.h"
#include "zigbee_config.h"
#include "zps_gen.h"
#include "app_ota_server.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_ZCL
#define TRACE_ZCL   FALSE
#endif

#define ZCL_TICK_TIME        ZTIMER_TIME_MSEC(100)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
static void APP_ZCL_cbGeneralCallback(tsZCL_CallBackEvent *psEvent);
static void APP_ZCL_cbEndpointCallback(tsZCL_CallBackEvent *psEvent);
static void APP_vHandleClusterCustomCommands(tsZCL_CallBackEvent *psEvent);
static void APP_vHandleClusterUpdate(tsZCL_CallBackEvent *psEvent);
static void APP_vZCL_DeviceSpecific_Init(void);
static void vStartEffect(uint8_t u8Effect);
static void vHandleIdentifyRequest(uint16_t u16Duration);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

tsZHA_BaseDevice sBaseDevice;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_ZCL_vInitialise
 *
 * DESCRIPTION:
 * Initialises ZCL related functions
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void APP_ZCL_vInitialise(void)
{
    teZCL_Status eZCL_Status;

    /* Initialise ZLL */
    eZCL_Status = eZCL_Initialise(&APP_ZCL_cbGeneralCallback, apduZCL);
    if (eZCL_Status != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ZCL, "\r\nErr: eZLO_Initialise:%d", eZCL_Status);
    }

    /* Start the tick timer */
    if(ZTIMER_eStart(u8TimerZCL, ZCL_TICK_TIME) != E_ZTIMER_OK)
    {
        DBG_vPrintf(TRACE_ZCL, "APP: Failed to Start Tick Timer\r\n");
    }

    /* Register Base Device EndPoint */
    eZCL_Status =  eZHA_RegisterBaseDeviceEndPoint(COORDINATOR_APPLICATION_ENDPOINT,
                                                   &APP_ZCL_cbEndpointCallback,
                                                   &sBaseDevice);
    if (eZCL_Status != E_ZCL_SUCCESS)
    {
            DBG_vPrintf(TRACE_ZCL, "Error: eZHA_RegisterBaseDeviceEndPoint:%02x\r\r\n", eZCL_Status);
    }

    APP_vZCL_DeviceSpecific_Init();
    vAppInitOTA();
}


/****************************************************************************
 *
 * NAME: APP_ZCL_vSetIdentifyTime
 *
 * DESCRIPTION:
 * Sets the remaining time in the identify cluster
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

void APP_ZCL_vSetIdentifyTime(uint16_t u16Time)
{
    sBaseDevice.sIdentifyServerCluster.u16IdentifyTime = u16Time;
}


/****************************************************************************
 *
 * NAME: APP_cbTimerZclTick
 *
 * DESCRIPTION:
 * Task kicked by the tick timer
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

void APP_cbTimerZclTick(void *pvParam)
{
    static uint32_t u32Tick10ms = 9;
    static uint32_t u32Tick1Sec = 99;
    tsZCL_CallBackEvent sCallBackEvent;

    if(ZTIMER_eStart(u8TimerZCL, ZTIMER_TIME_MSEC(10)) != E_ZTIMER_OK)
    {
        DBG_vPrintf(TRACE_ZCL, "APP: Failed to Start Tick Timer\r\n");
    }

    u32Tick10ms++;
    u32Tick1Sec++;

    /* Wrap the Tick10ms counter and provide 100ms ticks to cluster */
    if (u32Tick10ms > 9)
    {
        eZCL_Update100mS();
        u32Tick10ms = 0;
    }
    /* Wrap the 1 second  counter and provide 1Hz ticks to cluster */
    if(u32Tick1Sec > 99)
    {
        u32Tick1Sec = 0;
        sCallBackEvent.pZPSevent = NULL;
        sCallBackEvent.eEventType = E_ZCL_CBET_TIMER;
        vZCL_EventHandler(&sCallBackEvent);
    }
}

/****************************************************************************
 *
 * NAME: APP_ZCL_vEventHandler
 *
 * DESCRIPTION:
 * Main ZCL processing task
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void APP_ZCL_vEventHandler(ZPS_tsAfEvent *psStackEvent)
{
    tsZCL_CallBackEvent sCallBackEvent;
    sCallBackEvent.pZPSevent = psStackEvent;

    sCallBackEvent.eEventType = E_ZCL_CBET_ZIGBEE_EVENT;
    vZCL_EventHandler(&sCallBackEvent);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: APP_ZCL_cbGeneralCallback
 *
 * DESCRIPTION:
 * General callback for ZCL events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void APP_ZCL_cbGeneralCallback(tsZCL_CallBackEvent *psEvent)
{
#if TRUE == TRACE_ZCL
    switch (psEvent->eEventType)
    {

    case E_ZCL_CBET_LOCK_MUTEX:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: Lock Mutex\r\r\n");
        break;

    case E_ZCL_CBET_UNLOCK_MUTEX:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: Unlock Mutex\r\r\n");
        break;

    case E_ZCL_CBET_UNHANDLED_EVENT:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: Unhandled Event\r\r\n");
        break;

    case E_ZCL_CBET_READ_ATTRIBUTES_RESPONSE:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: Read attributes response");
        break;

    case E_ZCL_CBET_READ_REQUEST:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: Read request");
        break;

    case E_ZCL_CBET_DEFAULT_RESPONSE:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: Default response");
        break;

    case E_ZCL_CBET_ERROR:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: Error");
        break;

    case E_ZCL_CBET_TIMER:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: Timer");
        break;

    case E_ZCL_CBET_ZIGBEE_EVENT:
        DBG_vPrintf(TRACE_ZCL, "\r\nEVT: ZigBee");
        break;

    case E_ZCL_CBET_CLUSTER_CUSTOM:
        DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Custom");
        break;

    default:
        DBG_vPrintf(TRACE_ZCL, "\r\nInvalid event type");
        break;

    }
#endif

}


/****************************************************************************
 *
 * NAME: APP_ZCL_cbEndpointCallback
 *
 * DESCRIPTION:
 * Endpoint specific callback for ZCL events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void APP_ZCL_cbEndpointCallback(tsZCL_CallBackEvent *psEvent)
{

    switch (psEvent->eEventType)
    {
        case E_ZCL_CBET_LOCK_MUTEX:
            break;

        case E_ZCL_CBET_UNLOCK_MUTEX:
            break;

        case E_ZCL_CBET_UNHANDLED_EVENT:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Unhandled event");
            break;

        case E_ZCL_CBET_READ_INDIVIDUAL_ATTRIBUTE_RESPONSE:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Rd Attr Rsp %04x AS %d", psEvent->uMessage.sIndividualAttributeResponse.u16AttributeEnum,
                psEvent->uMessage.sIndividualAttributeResponse.eAttributeStatus);
            break;

        case E_ZCL_CBET_READ_ATTRIBUTES_RESPONSE:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Read attributes response");
            break;

        case E_ZCL_CBET_READ_REQUEST:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Read request");
            break;

        case E_ZCL_CBET_DEFAULT_RESPONSE:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Default response");
            break;

        case E_ZCL_CBET_ERROR:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Error");
            break;

        case E_ZCL_CBET_TIMER:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Timer");
            break;

        case E_ZCL_CBET_ZIGBEE_EVENT:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: ZigBee");
            break;

        case E_ZCL_CBET_CLUSTER_CUSTOM:
            APP_vHandleClusterCustomCommands(psEvent);
            break;

        case E_ZCL_CBET_WRITE_INDIVIDUAL_ATTRIBUTE:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Write Individual Attribute");
            break;

        case E_ZCL_CBET_CLUSTER_UPDATE:
            if(psEvent->psClusterInstance != NULL)
            {
                DBG_vPrintf(TRACE_ZCL, "Update Id %04x\r\n", psEvent->psClusterInstance->psClusterDefinition->u16ClusterEnum);
                APP_vHandleClusterUpdate(psEvent);
            }
            break;

        case E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTE:
            if(psEvent->psClusterInstance != NULL)
            {
                DBG_vPrintf(TRACE_ZCL, "ZCL Attribute Report: Cluster %04x Attribute %04x Value %d\r\n",
                            psEvent->pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                            psEvent->uMessage.sIndividualAttributeResponse.u16AttributeEnum,
                            *((uint8_t*)(psEvent->uMessage.sIndividualAttributeResponse.pvAttributeData)) );
            }
        break;

        case E_ZCL_CBET_REPORT_ATTRIBUTES:
            break;

        default:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Invalid evt type 0x%x", (uint8_t)psEvent->eEventType);
            break;
    }
}

/****************************************************************************
 *
 * NAME: APP_vHandleClusterCustomCommands
 *
 * DESCRIPTION:
 * callback for ZCL cluster custom command events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void APP_vHandleClusterCustomCommands(tsZCL_CallBackEvent *psEvent)
{

    switch(psEvent->uMessage.sClusterCustomMessage.u16ClusterId)
    {
        case GENERAL_CLUSTER_ID_BASIC:
        {
            tsCLD_BasicCallBackMessage *psCallBackMessage = (tsCLD_BasicCallBackMessage*)psEvent->uMessage.sClusterCustomMessage.pvCustomData;
            if (psCallBackMessage->u8CommandId == E_CLD_BASIC_CMD_RESET_TO_FACTORY_DEFAULTS )
            {
                DBG_vPrintf(TRACE_ZCL, "Basic Factory Reset Received\r\n");
                memset(&sBaseDevice,0,sizeof(tsZHA_BaseDevice));
                APP_vZCL_DeviceSpecific_Init();
                /* Register Base Device EndPoint */
                eZHA_RegisterBaseDeviceEndPoint(COORDINATOR_APPLICATION_ENDPOINT,
                                                               &APP_ZCL_cbEndpointCallback,
                                                               &sBaseDevice);
            }

        }
        break;

        case GENERAL_CLUSTER_ID_IDENTIFY:
        {
            tsCLD_IdentifyCallBackMessage *psCallBackMessage = (tsCLD_IdentifyCallBackMessage*)psEvent->uMessage.sClusterCustomMessage.pvCustomData;
            DBG_vPrintf(TRACE_ZCL, "- for identify cluster\r\r\n");
            /* provide callback to BDB handler for identify query response on initiator*/
            if(psEvent->psClusterInstance->bIsServer == FALSE)
            {
                tsBDB_ZCLEvent  sBDBZCLEvent;
                DBG_vPrintf(TRACE_ZCL, "\r\nCallBackBDB");
                sBDBZCLEvent.eType = BDB_E_ZCL_EVENT_IDENTIFY_QUERY;
                sBDBZCLEvent.psCallBackEvent = psEvent;
                BDB_vZclEventHandler(&sBDBZCLEvent);
            }
            else
            {
                // Server Side
                if (psCallBackMessage->u8CommandId == E_CLD_IDENTIFY_CMD_IDENTIFY)
                {
                    vHandleIdentifyRequest(sBaseDevice.sIdentifyServerCluster.u16IdentifyTime);
                }
                else if (psCallBackMessage->u8CommandId == E_CLD_IDENTIFY_CMD_TRIGGER_EFFECT)
                {
                    vStartEffect( psCallBackMessage->uMessage.psTriggerEffectRequestPayload->eEffectId);
                }
            }
        }
        break;


        case OTA_CLUSTER_ID:
        {
            tsOTA_CallBackMessage *psCallBackMessage = (tsOTA_CallBackMessage *)psEvent->uMessage.sClusterCustomMessage.pvCustomData;
            if(psCallBackMessage->eEventId == E_CLD_OTA_COMMAND_UPGRADE_END_REQUEST )
            {
                tsCLD_PR_Ota               sOTAData;
                DBG_vPrintf(TRACE_ZCL, "\nUpgrade End Req received from %x status %x...", psEvent->pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr,
                    		                                                              psCallBackMessage->uMessage.sUpgradeEndRequestPayload.u8Status);
                eOTA_GetServerData(APP_u8GetDeviceEndpoint(),0,&sOTAData);
                sOTAData.u32CurrentTime=0;
                sOTAData.u32RequestOrUpgradeTime =5;
                eOTA_SetServerParams(APP_u8GetDeviceEndpoint(),0,&sOTAData);
            }
        }
        break;
    }
}

/****************************************************************************
 *
 * NAME: APP_vHandleClusterUpdate
 *
 * DESCRIPTION:
 * callback for ZCL cluster update events
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void APP_vHandleClusterUpdate(tsZCL_CallBackEvent *psEvent)
{
    if (psEvent->psClusterInstance->psClusterDefinition->u16ClusterEnum == GENERAL_CLUSTER_ID_IDENTIFY)
    {
        if(sBaseDevice.sIdentifyServerCluster.u16IdentifyTime == 0)
        {
            tsBDB_ZCLEvent  sBDBZCLEvent;
            /* provide callback to BDB handler for identify on Target */
            sBDBZCLEvent.eType = BDB_E_ZCL_EVENT_IDENTIFY;
            sBDBZCLEvent.psCallBackEvent = psEvent;
            BDB_vZclEventHandler(&sBDBZCLEvent);
        }
    }
}

/****************************************************************************
 *
 * NAME: APP_vZCL_DeviceSpecific_Init
 *
 * DESCRIPTION:
 * ZCL specific initialization
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void APP_vZCL_DeviceSpecific_Init(void)
{
    memcpy(sBaseDevice.sBasicServerCluster.au8ManufacturerName, "NXP", CLD_BAS_MANUF_NAME_SIZE);
    memcpy(sBaseDevice.sBasicServerCluster.au8ModelIdentifier, "BDB-Coordinator", CLD_BAS_MODEL_ID_SIZE);
    memcpy(sBaseDevice.sBasicServerCluster.au8DateCode, "20150212", CLD_BAS_DATE_SIZE);
    memcpy(sBaseDevice.sBasicServerCluster.au8SWBuildID, "1000-0001", CLD_BAS_SW_BUILD_SIZE);
}

/****************************************************************************
 *
 * NAME: vStartEffect
 *
 * DESCRIPTION:
 * ZLO Device Specific identify effect set up
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
static void vStartEffect(uint8_t u8Effect)
{
    switch (u8Effect) {
        case E_CLD_IDENTIFY_EFFECT_BLINK:
            sBaseDevice.sIdentifyServerCluster.u16IdentifyTime = 2;
            break;

        case E_CLD_IDENTIFY_EFFECT_BREATHE:
            sBaseDevice.sIdentifyServerCluster.u16IdentifyTime = 17;
            break;

        case E_CLD_IDENTIFY_EFFECT_OKAY:
            sBaseDevice.sIdentifyServerCluster.u16IdentifyTime = 3;
            break;

        case E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE:
            sBaseDevice.sIdentifyServerCluster.u16IdentifyTime = 9;
            break;

        case E_CLD_IDENTIFY_EFFECT_FINISH_EFFECT:
        case E_CLD_IDENTIFY_EFFECT_STOP_EFFECT:
            sBaseDevice.sIdentifyServerCluster.u16IdentifyTime = 1;
            break;
    }
    vHandleIdentifyRequest( sBaseDevice.sIdentifyServerCluster.u16IdentifyTime);
}

/****************************************************************************
 *
 * NAME: vHandleIdentifyRequest
 *
 * DESCRIPTION: handle identify request command received by the remote
 * causes the identify blink for the required time
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void vHandleIdentifyRequest(uint16_t u16Duration)
{
    ZTIMER_eStop(u8TimerId);
    if (u16Duration == 0)
    {
        DBG_vPrintf(TRUE, "Identifying stopped\n");
    }
    else
    {
        ZTIMER_eStart(u8TimerId, ZTIMER_TIME_MSEC(500));
        DBG_vPrintf(TRUE, "Identifying\n");
    }
}

/****************************************************************************
 *
 * NAME: APP_cbTimerId
 *
 * DESCRIPTION: Tasks that handles the flashing leds during identfy operation
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void APP_cbTimerId(void *pvParam)
{
    if (sBaseDevice.sIdentifyServerCluster.u16IdentifyTime == 0)
    {
        ZTIMER_eStop(u8TimerId);
        DBG_vPrintf(TRUE, "Identifying stopped\n");
    }
    else
    {
        ZTIMER_eStop(u8TimerId);
        ZTIMER_eStart(u8TimerId, ZTIMER_TIME_MSEC(500));
        DBG_vPrintf(TRUE, "Identifying\n");
    }
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
