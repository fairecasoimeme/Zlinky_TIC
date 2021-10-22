/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "zcl.h"
#include "zps_gen.h"
#include "pdum_gen.h"
#include "base_device.h"
#include "ZTimer.h"
#include "dbg.h"
#include "zigbee_config.h"
#include "app_ota_client.h"
#include "app_leds.h"
#include "app.h"
#include "app_common.h"
#include "app_reporting.h"
#include "bdb_api.h"
#include "app_main.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_ZCL
#define TRACE_ZCL   TRUE
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
void APP_vHandleIdentify(uint16_t u16Time);
static void APP_vHandleClusterCustomCommands(tsZCL_CallBackEvent *psEvent);
static void APP_vHandleClusterUpdate(tsZCL_CallBackEvent *psEvent);
static void APP_vZCL_DeviceSpecific_Init(void);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

tsZHA_BaseDevice sBaseDevice;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static uint8_t u8IdentifyCount = 0;
static bool_t bIdentifyState = FALSE;


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

    /* Register Light EndPoint */
   eZCL_Status =  eZHA_RegisterBaseDeviceEndPoint(APP_u8GetDeviceEndpoint(),
                                                   &APP_ZCL_cbEndpointCallback,
                                                   &sBaseDevice);
    if (eZCL_Status != E_ZCL_SUCCESS)
    {
            DBG_vPrintf(TRACE_ZCL, "Error: eZHA_RegisterBaseDeviceEndPoint: %02x\r\r\n", eZCL_Status);
    }

    APP_vZCL_DeviceSpecific_Init();

    vAppInitOTA();
    APP_vSetLed(LED1, sBaseDevice.sOnOffServerCluster.bOnOff);

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
    static uint32_t u32RadioTempUpdateMs = 0;

    tsZCL_CallBackEvent sCallBackEvent;

    if(ZTIMER_eStart(u8TimerZCL, ZTIMER_TIME_MSEC(10)) != E_ZTIMER_OK)
    {
        DBG_vPrintf(TRACE_ZCL, "APP: Failed to Start Tick Timer\r\n");
    }

    u32Tick10ms++;
    u32Tick1Sec++;
    u32RadioTempUpdateMs += 10; /* 10ms for normal timer use */

    /* Wrap the Tick10ms counter and provide 100ms ticks to cluster */
    if (u32Tick10ms > 9)
    {
        eZCL_Update100mS();
        u32Tick10ms = 0;
    }

#ifdef CLD_OTA
    if (u32Tick1Sec == 82)   /* offset this from the 1 second roll over */
    {
        vRunAppOTAStateMachine(1000);
    }
#endif

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

    //DBG_vPrintf(TRACE_ZCL, "ZCL_Task endpoint event:%d \r\n", psStackEvent->eType);
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
            if(psEvent->psClusterInstance != NULL)
            {
                APP_vHandleClusterCustomCommands(psEvent);
            }
        break;

        case E_ZCL_CBET_WRITE_INDIVIDUAL_ATTRIBUTE:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Write Individual Attribute Status %02x\r\n", psEvent->eZCL_Status);
        break;

        case E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTE:
        {
            tsZCL_IndividualAttributesResponse    *psIndividualAttributeResponse = &psEvent->uMessage.sIndividualAttributeResponse;
            DBG_vPrintf(TRACE_ZCL,"Individual Report attribute for Cluster = %d\r\n", psEvent->psClusterInstance->psClusterDefinition->u16ClusterEnum);
            DBG_vPrintf(TRACE_ZCL,"eAttributeDataType = %d\r\n", psIndividualAttributeResponse->eAttributeDataType);
            DBG_vPrintf(TRACE_ZCL,"u16AttributeEnum = %d\r\n", psIndividualAttributeResponse->u16AttributeEnum );
            DBG_vPrintf(TRACE_ZCL,"eAttributeStatus = %d\r\n", psIndividualAttributeResponse->eAttributeStatus );
        }
            break;

        case E_ZCL_CBET_REPORT_INDIVIDUAL_ATTRIBUTES_CONFIGURE:
        {
            if(psEvent->psClusterInstance != NULL)
            {
                tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord = &psEvent->uMessage.sAttributeReportingConfigurationRecord;
                DBG_vPrintf(TRACE_ZCL,"Individual Configure Report Cluster %d Attrib %d Type %d Min %d Max %d IntV %d Direcct %d Change %d\r\n",
                    psEvent->psClusterInstance->psClusterDefinition->u16ClusterEnum,
                    psAttributeReportingRecord->u16AttributeEnum,
                    psAttributeReportingRecord->eAttributeDataType,
                    psAttributeReportingRecord->u16MinimumReportingInterval,
                    psAttributeReportingRecord->u16MaximumReportingInterval,
                    psAttributeReportingRecord->u16TimeoutPeriodField,
                    psAttributeReportingRecord->u8DirectionIsReceived,
                    psAttributeReportingRecord->uAttributeReportableChange);

                if (E_ZCL_SUCCESS == psEvent->eZCL_Status)
                {

                    vSaveReportableRecord( psEvent->psClusterInstance->psClusterDefinition->u16ClusterEnum,
                                       psAttributeReportingRecord);

                }
            }
        }
        break;

        case E_ZCL_CBET_CLUSTER_UPDATE:
            if(psEvent->psClusterInstance != NULL)
            {
                DBG_vPrintf(TRACE_ZCL, "Update Id %04x\r\n", psEvent->psClusterInstance->psClusterDefinition->u16ClusterEnum);
                APP_vHandleClusterUpdate(psEvent);
            }
            break;
        case E_ZCL_CBET_REPORT_REQUEST:
            break;

        default:
            DBG_vPrintf(TRACE_ZCL, "\r\nEP EVT: Invalid evt type 0x%x", (uint8_t)psEvent->eEventType);
            break;
    }
}

/****************************************************************************
 *
 * NAME: APP_vHandleIdentify
 *
 * DESCRIPTION:
 * Application identify handler
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
void APP_vHandleIdentify(uint16_t u16Time)
{
    static bool bActive = FALSE;
    if (u16Time == 0)
    {
            /*
             * Restore to off/off state
             */
        APP_vSetLed(LED1, sBaseDevice.sOnOffServerCluster.bOnOff);
        bActive = FALSE;
    }
    else
    {
        /* Set the Identify levels */
        if (!bActive) {
            bActive = TRUE;
            u8IdentifyCount = 5;
            bIdentifyState = TRUE;
            APP_vSetLed(LED1, TRUE );
        }
    }
}

#ifdef CLD_IDENTIFY_10HZ_TICK
/****************************************************************************
 *
 * NAME: vIdEffectTick
 *
 * DESCRIPTION:
 * ZLO Device Specific identify tick
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
void vIdEffectTick(uint8_t u8Endpoint)
{

    if ((u8Endpoint == APP_u8GetDeviceEndpoint()) &&
        (sBaseDevice.sIdentifyServerCluster.u16IdentifyTime > 0 ))
    {
        u8IdentifyCount--;
        if (u8IdentifyCount == 0)
        {
            u8IdentifyCount = 5;
            bIdentifyState = (bIdentifyState)? FALSE: TRUE;
            APP_vSetLed(LED1, bIdentifyState);
        }
    }
}
#endif

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
        case GENERAL_CLUSTER_ID_ONOFF:
        {
            APP_vSetLed(LED1, sBaseDevice.sOnOffServerCluster.bOnOff);
        }
        break;

        case GENERAL_CLUSTER_ID_IDENTIFY:
        {
            tsCLD_IdentifyCallBackMessage *psCallBackMessage = (tsCLD_IdentifyCallBackMessage*)psEvent->uMessage.sClusterCustomMessage.pvCustomData;
            if (psCallBackMessage->u8CommandId == E_CLD_IDENTIFY_CMD_IDENTIFY)
            {
                APP_vHandleIdentify(sBaseDevice.sIdentifyServerCluster.u16IdentifyTime);
            }
        }
        break;

        case GENERAL_CLUSTER_ID_BASIC:
        {
            tsCLD_BasicCallBackMessage *psCallBackMessage = (tsCLD_BasicCallBackMessage*)psEvent->uMessage.sClusterCustomMessage.pvCustomData;
            if (psCallBackMessage->u8CommandId == E_CLD_BASIC_CMD_RESET_TO_FACTORY_DEFAULTS )
            {
                DBG_vPrintf(TRACE_ZCL, "Basic Factory Reset Received\r\n");
                memset(&sBaseDevice,0,sizeof(tsZHA_BaseDevice));
                APP_vZCL_DeviceSpecific_Init();
                vAppInitOTA();
                eZHA_RegisterBaseDeviceEndPoint(APP_u8GetDeviceEndpoint(),
                                                &APP_ZCL_cbEndpointCallback,
                                                &sBaseDevice);

            }
        }
        break;

        case OTA_CLUSTER_ID:
        {
            tsOTA_CallBackMessage *psCallBackMessage = (tsOTA_CallBackMessage *)psEvent->uMessage.sClusterCustomMessage.pvCustomData;
            if(psCallBackMessage->eEventId == E_CLD_OTA_COMMAND_BLOCK_RESPONSE)
            {
                u32Togglems = 500;
                ZTIMER_eStop(u8LedTimer);
                ZTIMER_eStart(u8LedTimer, ZTIMER_TIME_MSEC(u32Togglems));
            }

            if(psCallBackMessage->eEventId ==  E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED)
            {
                u32Togglems = 2000;
                ZTIMER_eStop(u8LedTimer);
                ZTIMER_eStart(u8LedTimer, ZTIMER_TIME_MSEC(u32Togglems));
            }
            vHandleAppOtaClient(psCallBackMessage);
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
        APP_vHandleIdentify(sBaseDevice.sIdentifyServerCluster.u16IdentifyTime);
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
    sBaseDevice.sOnOffServerCluster.bOnOff = FALSE;
    memcpy(sBaseDevice.sBasicServerCluster.au8ManufacturerName, "NXP", CLD_BAS_MANUF_NAME_SIZE);
    memcpy(sBaseDevice.sBasicServerCluster.au8ModelIdentifier, "BDB-EndDevice", CLD_BAS_MODEL_ID_SIZE);
    memcpy(sBaseDevice.sBasicServerCluster.au8DateCode, "20191120", CLD_BAS_DATE_SIZE);
    memcpy(sBaseDevice.sBasicServerCluster.au8SWBuildID, "1000-0001", CLD_BAS_SW_BUILD_SIZE);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
