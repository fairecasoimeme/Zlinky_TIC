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


/****************************************************************************
 *
 * MODULE:             Zigbee Demand Response And Load Control Cluster
 *
 * DESCRIPTION:
 * DRLC server command handlers
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "DRLC.h"
#include "DRLC_internal.h"
#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vSendScheduledListEvents(
                            ZPS_tsAfEvent                *pZPSevent,
                            tsZCL_EndPointDefinition     *psEndPointDefinition,
                            tsZCL_ClusterInstance        *psClusterInstance,
                            tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure,
                            tsZCL_CallBackEvent          *psDRLCCallBackEvent,
                            teSE_DRLCEventList            eEventList,
                            uint32                        u32StartTime,
                            uint8                         *pu8NumberOfEvents);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       vSE_DRLCHandleReportEventStatus
 **
 ** DESCRIPTION:
 ** Handles DRLC Scheduled Report events
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC  void vSE_DRLCHandleReportEventStatus(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    uint8 u8TransactionSequenceNumber;
    teSE_DRLCStatus eDRLCStatus;

    tsDRLC_Common *psDRLC_Common;

    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;

    // get load control informations
    eDRLCStatus = eSE_DRLCReportEventStatusReceive(pZPSevent, psEndPointDefinition, &psDRLC_Common->sDRLCCallBackMessage.uMessage.sReportEvent, &u8TransactionSequenceNumber);

    // get mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psDRLC_Common->sDRLCCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    // call user
    eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_REPORT_EVENT_STATUS, eDRLCStatus);

    // call user
    psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


}

/****************************************************************************
 **
 ** NAME:       vSE_DRLCHandleGetScheduledEvents
 **
 ** DESCRIPTION:
 ** Handles DRLC Scheduled events request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC  void vSE_DRLCHandleGetScheduledEvents(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    uint8 u8NumberOfEvents, u8NumberOfEventsAskedFor;

    uint8 u8TransactionSequenceNumber;

    uint8 u8DRLCStatus;

    tsSE_DRLCCallBackMessage sDRLCCallBackMessage;
    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;
    uint32 u32StartTime;

    tsDRLC_Common *psDRLC_Common;

    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;

    // get schedule request
    u8DRLCStatus = eSE_DRLCGetScheduledEventsReceive(pZPSevent, psEndPointDefinition, &sDRLCCallBackMessage.uMessage.sGetScheduledEvents, &u8TransactionSequenceNumber);
    if(u8DRLCStatus != E_ZCL_SUCCESS)
    {
        // nothing we can do
        return;
    }

    // get mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // initialise custom data pointer
    psDRLCCustomDataStructure = (tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psDRLC_Common->sDRLCCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    // call user
    eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_GET_SCHEDULED_EVENTS, u8DRLCStatus);

    // zero means send all available events
    u8NumberOfEvents = sDRLCCallBackMessage.uMessage.sGetScheduledEvents.u8numberOfEvents;
    if (u8NumberOfEvents == 0)
    {
        u8NumberOfEvents = 255;
    }
    u32StartTime = sDRLCCallBackMessage.uMessage.sGetScheduledEvents.u32StartTime;
    u8NumberOfEventsAskedFor = u8NumberOfEvents;

    // send all list events
    vSendScheduledListEvents(pZPSevent, psEndPointDefinition,  psClusterInstance, psDRLCCustomDataStructure, &psDRLC_Common->sDRLCCustomCallBackEvent, E_SE_DRLC_EVENT_LIST_ACTIVE, u32StartTime, &u8NumberOfEvents);
    vSendScheduledListEvents(pZPSevent, psEndPointDefinition,  psClusterInstance, psDRLCCustomDataStructure, &psDRLC_Common->sDRLCCustomCallBackEvent, E_SE_DRLC_EVENT_LIST_SCHEDULED, u32StartTime, &u8NumberOfEvents);

    // No events sent - reply with a default message
    if(u8NumberOfEventsAskedFor == u8NumberOfEvents)
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
    }



    // call user
    psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


}

/***************************************************************************
 **
 ** NAME:       vSendScheduledListEvents
 **
 ** DESCRIPTION:
 ** Sends scheduled events.
 ** Events must be >= start time of psDRLCCustomDataStructure.and until num events is zero)
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_EndPointDefinition        *psEndPointDefinition       EP structure
 ** tsZCL_ClusterInstance           *psClusterInstance          Cluster structure
 ** tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure  list struct
 ** tsZCL_CallBackEvent             *psDRLCCallBackEvent        Callback
 ** teSE_DRLCEventList              eEventList                  List to send from
 ** uint32                          u32StartTime                Send events with start after this time
 ** uint8                           *pu8NumberOfEvents          Pointer to num events to send - decremented on each send and don't send when zero
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  void vSendScheduledListEvents(
                            ZPS_tsAfEvent                *pZPSevent,
                            tsZCL_EndPointDefinition     *psEndPointDefinition,
                            tsZCL_ClusterInstance        *psClusterInstance,
                            tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure,
                            tsZCL_CallBackEvent          *psDRLCCallBackEvent,
                            teSE_DRLCEventList            eEventList,
                            uint32                        u32StartTime,
                            uint8                         *pu8NumberOfEvents)
{

    tsZCL_Address       sZCL_Address;

    tsSE_DRLCLoadControlEventRecord *psHeadLoadControlEventRecord;

    DLIST *plEventList;

    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);

    // get head of each list - cancel each event
    psHeadLoadControlEventRecord = psGetListHead(psDRLCCustomDataStructure, eEventList, &plEventList);

    while(psHeadLoadControlEventRecord!=NULL)
    {
        if (*pu8NumberOfEvents == 0)
        {
            return;
        }

        /* Compare with end time of event (D.2.3.3.2)*/
        if ( ((psHeadLoadControlEventRecord->sLoadControlEvent.u32StartTime) +
                psHeadLoadControlEventRecord->sLoadControlEvent.u16DurationInMinutes*60) >= u32StartTime)
        {

            // send entry
             eSE_DRLCLoadControlEventSend(
                pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,  // Swap src and dst for reply
                pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                &sZCL_Address,
                &psHeadLoadControlEventRecord->sLoadControlEvent,
                FALSE,
                &psDRLCCallBackEvent->u8TransactionSequenceNumber);

            (*pu8NumberOfEvents)--;
        }
        // get next list item
        psHeadLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetNext((DNODE *)&psHeadLoadControlEventRecord->dllrlcNode);
    }
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
