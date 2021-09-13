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
 * DESCRIPTION:        DRLC table manager
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "DRLC.h"
#include "DRLC_internal.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE bool_t bGetEarliestStartTime(
                tsSE_DRLCLoadControlEventRecord   *psLoadControlEventAddRecord,
                tsSE_DRLCLoadControlEventRecord   *psLoadControlEventRecord,
                tsSE_DRLCLoadControlEventRecord   **ppsEarliestLoadControlEventRecord,
                tsSE_DRLCLoadControlEventRecord   **ppsLatestLoadControlEventRecord);

PRIVATE bool_t boSearchForExisting(
                void                          		*pvSearchParam,
                void                                *psNodeUnderTest);



PRIVATE bool_t bCheckForEventTimeOverlap(
                tsSE_DRLCLoadControlEventRecord   *psLoadControlEventAddRecord,
                tsSE_DRLCLoadControlEventRecord   *psLoadControlEventReferenceRecord);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eSE_DRLCAddLoadControlEventNoMutex
 **
 ** DESCRIPTION:
 ** Add load control event
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_EndPointDefinition        *psEndPointDefinition       EP structure
 ** tsZCL_ClusterInstance           *psClusterInstance          Cluster structure
 ** tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure  list access
 ** tsLoadControlEvent              *psLoadControlEvent         table data
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCAddLoadControlEventNoMutex(
                                tsZCL_EndPointDefinition     *psEndPointDefinition,
                                tsZCL_ClusterInstance        *psClusterInstance,
                                tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure,
                                tsSE_DRLCLoadControlEvent    *psLoadControlEvent)
{

    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;
    teSE_DRLCEventList eEventList;

    uint32 u32UTCtime =  u32ZCL_GetUTCTime();

    teSE_DRLCEventList eToEventList = E_SE_DRLC_EVENT_LIST_SCHEDULED;

#ifndef DRLC_TEST_HARNESS
    // check to see if the entry exists already
    psLoadControlEventRecord = psCheckToSeeIfEventExists(psDRLCCustomDataStructure, psLoadControlEvent->u32IssuerId, &eEventList);
    // return if error
    if(psLoadControlEventRecord!=NULL)
    {
        return(E_SE_DRLC_DUPLICATE_EXISTS);
    }

    if(u32UTCtime > (psLoadControlEvent->u32StartTime + psLoadControlEvent->u16DurationInMinutes*60))
    {
        return(E_SE_DRLC_EVENT_LATE);
    }
#endif
    // we need to get a list item from the dealloc list
    psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead(&psDRLCCustomDataStructure->lLoadControlEventDeAllocList);
    if(psLoadControlEventRecord==NULL)
    {
        return E_ZCL_ERR_INSUFFICIENT_SPACE;
    }

    // initialise user opt-in-out event status
    psLoadControlEventRecord->eEventOption = E_SE_DRLC_EVENT_USER_OPT_IN;
    psLoadControlEventRecord->u8EventOptionChangeCount = 0;
    psLoadControlEventRecord->sLoadControlEvent.u8EventControl = psLoadControlEvent->u8EventControl;
    // initialise user event values
    eSE_DRLCInitialiseReportStructure(&psLoadControlEventRecord->sReportEvent,
        psLoadControlEvent->u32IssuerId,
        psLoadControlEvent->u8CriticalityLevel,
        psLoadControlEvent->u8EventControl);

    // set randomization values
    vSetRandomizationValues(psEndPointDefinition, psClusterInstance, psLoadControlEventRecord);

#ifndef DRLC_TEST_HARNESS
    // add randomization minutes in terms of seconds
    if(u32UTCtime >= ((psLoadControlEvent->u32StartTime) + ((psLoadControlEventRecord->u8StartRandomizationMinutes)*60)))
    {
        // make effective time 'now' modify duration accordingly and carry on
        psLoadControlEvent->u32StartTime = u32UTCtime;
        psLoadControlEvent->u16DurationInMinutes -= ((uint16)(u32UTCtime - psLoadControlEvent->u32StartTime)/60);
        //eToEventList = E_SE_DRLC_EVENT_LIST_ACTIVE;
    }
#endif

    // set default cancel mode
    psLoadControlEventRecord->eCancelControl = E_SE_DRLC_CANCEL_CONTROL_IMMEDIATE;
    // set effective cancel time to zero
    psLoadControlEventRecord->u32EffectiveCancelTime = 0;
    // clear deferred flag
    psLoadControlEventRecord->bDeferredCancel = FALSE;

    // search list for correct add position
    memcpy(&psLoadControlEventRecord->sLoadControlEvent, psLoadControlEvent, sizeof(tsSE_DRLCLoadControlEvent));

    // add
    return eSE_DRLCAddToSchedulerList(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, psLoadControlEventRecord, E_SE_DRLC_EVENT_LIST_DEALLOCATED, eToEventList);

}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCInitialiseReportStructure
 **
 ** DESCRIPTION:
 ** Initialise Report Structure
 **
 ** PARAMETERS:               Name                  Usage
 ** tsSE_DRLCReportEvent      psReportEvent         report structure
 ** uint32                    u32IssuerId           issuer id
 ** uint8                     u8CriticalityLevel    Criticality level which should reflect the incoming event
 ** uint8                     u8EventControl        Event control which should reflect the incoming event
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCInitialiseReportStructure(
                                tsSE_DRLCReportEvent    *psReportEvent,
                                uint32                  u32IssuerId,
                                uint8                   u8CriticalityLevel,
                                uint8                   u8EventControl)
{
    if(psReportEvent==NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }
    // initialise user event values
    psReportEvent->u32IssuerId = u32IssuerId;
    psReportEvent->u8CriticalityLevelApplied = u8CriticalityLevel;
    psReportEvent->u8EventControl = u8EventControl;
    psReportEvent->u8EventStatus = 0; // Reserved value - real status must be filled in later.
    psReportEvent->u16CoolingTemperatureSetPointApplied = 0x8000;
    psReportEvent->u16HeatingTemperatureSetPointApplied = 0x8000;
    psReportEvent->u8AverageLoadAdjustmentPercentageApplied = 0x80;
    psReportEvent->u8DutyCycleApplied = 0xff;

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eSE_DRLCCancelLoadControlEventNoMutex
 **
 ** DESCRIPTION:
 ** cancel load control event
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_EndPointDefinition        *psEndPointDefinition       EP structure
 ** tsZCL_ClusterInstance           *psClusterInstance          Cluster structure
 ** tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure  list access
 ** tsSE_DRLCCancelLoadControlEvent *psCancelLoadControlEvent   canceltable data
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/
PUBLIC  teSE_DRLCStatus eSE_DRLCCancelLoadControlEventNoMutex(
                            tsZCL_EndPointDefinition           *psEndPointDefinition,
                            tsZCL_ClusterInstance              *psClusterInstance,
                            tsSE_DRLCCustomDataStructure       *psDRLCCustomDataStructure,
                            tsSE_DRLCCancelLoadControlEvent    *psCancelLoadControlEvent)
{

    teSE_DRLCEventList eEventList;
    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;

    // check each list to find the event
    psLoadControlEventRecord = psCheckToSeeIfEventExists(psDRLCCustomDataStructure, psCancelLoadControlEvent->u32IssuerId, &eEventList);
    if(psLoadControlEventRecord==NULL)
    {
        return E_SE_DRLC_NOT_FOUND;
    }

    if(eEventList== E_SE_DRLC_EVENT_LIST_CANCELLED)
    {
        return(E_ZCL_SUCCESS);
    }

    if (psCancelLoadControlEvent->u32effectiveTime == 0)
    {
        psCancelLoadControlEvent->u32effectiveTime = u32ZCL_GetUTCTime();
    }

    // check for validity
    if(psCancelLoadControlEvent->u32effectiveTime > u32EffectiveDuration(psLoadControlEventRecord, psEndPointDefinition, psClusterInstance))
    {
        // event will expire naturally before stated cancel time
        return(E_ZCL_FAIL);
    }

    // check for lateness or nearly late
    if((psCancelLoadControlEvent->eCancelControl == E_SE_DRLC_CANCEL_CONTROL_IMMEDIATE) &&
        (psCancelLoadControlEvent->u32effectiveTime <= u32ZCL_GetUTCTime())
    )
    {
        // remove immediately
        vSimpleAddEventToScheduler(psDRLCCustomDataStructure, psLoadControlEventRecord, eEventList, E_SE_DRLC_EVENT_LIST_DEALLOCATED);
    }
    else
    {
        // write in the cancel time to the event and store it on the cancel list if it is already active
        psLoadControlEventRecord->u32EffectiveCancelTime =
            psCancelLoadControlEvent->u32effectiveTime + ( psLoadControlEventRecord->u8DurationRandomizationMinutes * 60 );

        psLoadControlEventRecord->eCancelControl = psCancelLoadControlEvent->eCancelControl;
        psLoadControlEventRecord->bDeferredCancel = TRUE;
        // only move to cancelled list when active
        if(eEventList==E_SE_DRLC_EVENT_LIST_ACTIVE)
        {
            // remove and add to cancelled
            vSimpleAddEventToScheduler(psDRLCCustomDataStructure, psLoadControlEventRecord, E_SE_DRLC_EVENT_LIST_ACTIVE, E_SE_DRLC_EVENT_LIST_CANCELLED);
        }

        return(E_SE_DRLC_CANCEL_DEFERRED);
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCOverlapAddEventToScheduler
 **
 ** DESCRIPTION:
 ** Add overlapping event
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_EndPointDefinition        *psEndPointDefinition       EP structure
 ** tsZCL_ClusterInstance           *psClusterInstance          Cluster structure
 ** tsLoadControlEvent              *psLoadControlEvent         table data
 ** tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure  list access
 ** teSE_DRLCEventList              eEventList                  list to search
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCOverlapAddEventToScheduler(
                tsZCL_EndPointDefinition              *psEndPointDefinition,
                tsZCL_ClusterInstance                 *psClusterInstance,
                tsSE_DRLCLoadControlEventRecord       *psLoadControlEventAddRecord,
                tsSE_DRLCCustomDataStructure          *psDRLCCustomDataStructure,
                teSE_DRLCEventList                     eFromEventList,
                teSE_DRLCEventList                     eToEventList)
{
    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;
    tsSE_DRLCLoadControlEvent *psLoadControlEvent;

    tsSE_DRLCLoadControlEvent   *psLoadControlEventAdd;
    tsSE_DRLCLoadControlEventRecord   *psEarliestLoadControlEventRecord;
    tsSE_DRLCLoadControlEventRecord   *psLatestLoadControlEventRecord;

    bool_t bEventTimeOverlap;
    bool_t bLoadControlEventAddEarliest;

    uint16  u16CancelledDeviceClassBits;

    tsSE_DRLCReportEvent sReportEvent;

    DLIST *plToEventList;

    tsDRLC_Common *psDRLC_Common;

    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;

    psLoadControlEventAdd = &psLoadControlEventAddRecord->sLoadControlEvent;
    // get start of list
    psLoadControlEventRecord =  psGetListHead(psDRLCCustomDataStructure, eToEventList, &plToEventList);


    // check contents of list
    bEventTimeOverlap = FALSE;

    // fill in callback event structure if its the client only
    if(psClusterInstance->bIsServer==FALSE)
    {
        // fill in callback event structure
        eZCL_SetCustomCallBackEvent(&psDRLC_Common->sDRLCCustomCallBackEvent, 0, 0, psEndPointDefinition->u8EndPointNumber);
        eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT, E_ZCL_SUCCESS);
    }

    while(psLoadControlEventRecord != NULL)
    {
        psLoadControlEvent = &psLoadControlEventRecord->sLoadControlEvent;

        bEventTimeOverlap = FALSE;
        // check device ID and enrolment params to see if we need to a match check
        if((psLoadControlEvent->u16DeviceClass & psLoadControlEventAdd->u16DeviceClass) ||
           (psLoadControlEvent->u8UtilityEnrolmentGroup == psLoadControlEventAdd->u8UtilityEnrolmentGroup) ||
           (psLoadControlEventAdd->u8UtilityEnrolmentGroup == 0)
        )
        {
            bEventTimeOverlap = bCheckForEventTimeOverlap(psLoadControlEventAddRecord, psLoadControlEventRecord);
        }

        if(bEventTimeOverlap==TRUE)
        {
            bLoadControlEventAddEarliest =
            bGetEarliestStartTime(psLoadControlEventAddRecord, psLoadControlEventRecord, &psEarliestLoadControlEventRecord, &psLatestLoadControlEventRecord);

            // if simple match
            if((psLoadControlEventAdd->u16DeviceClass == psLoadControlEvent->u16DeviceClass) && (bLoadControlEventAddEarliest==TRUE))
            {
                // don't do anything to the list - reject the event
                // initialise local event subfields
                eSE_DRLCInitialiseReportStructure(&sReportEvent, psLoadControlEventAdd->u32IssuerId, psLoadControlEventAdd->u8CriticalityLevel, psLoadControlEventAdd->u8EventControl);
                sReportEvent.u8EventStatus = E_SE_DRLC_LOAD_CONTROL_EVENT_COMMAND_REJECTED;
                // send user event
                eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance, &sReportEvent);
                return(E_ZCL_FAIL);
            }

            // partial match - the spec isn't so clear on this ....
            // remove the clashing device Id bits and define it as non-overlapping remove the oldest one - earliest start time
            // but what do we signal to the user for the clashing event bits - my interpretation is a c/b with the matching bits in the
            // device id indicating that the event has ended for those bits only - u16CancelledDeviceClassBits
            u16CancelledDeviceClassBits = psEarliestLoadControlEventRecord->sLoadControlEvent.u16DeviceClass & psLatestLoadControlEventRecord->sLoadControlEvent.u16DeviceClass;
            psEarliestLoadControlEventRecord->sLoadControlEvent.u16DeviceClass ^= psLatestLoadControlEventRecord->sLoadControlEvent.u16DeviceClass;

            // build user callback
            if(psClusterInstance->bIsServer==FALSE)
            {
                eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_CANCELLED, SE_DRLC_LOAD_CONTROL_EVENT_CANCEL, E_ZCL_SUCCESS);
                // craft a tsSE_DRLCCancelLoadControlEvent
                psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u32IssuerId = psEarliestLoadControlEventRecord->sLoadControlEvent.u32IssuerId;
                psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u16DeviceClass = u16CancelledDeviceClassBits;
                psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u8UtilityEnrolmentGroup = psEarliestLoadControlEventRecord->sLoadControlEvent.u8UtilityEnrolmentGroup;
                psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.eCancelControl = E_SE_DRLC_CANCEL_CONTROL_IMMEDIATE;
                psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u32effectiveTime = 0;
                // user callback
                psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
            }

            if(psEarliestLoadControlEventRecord->sLoadControlEvent.u16DeviceClass==0)
            {
                if(bLoadControlEventAddEarliest==FALSE)
                {
                    // remove the earliest event
                    psDLISTremove(plToEventList, (DNODE *)psEarliestLoadControlEventRecord);
                    // add to tail of dealloc list
                    vDLISTaddToTail(&psDRLCCustomDataStructure->lLoadControlEventDeAllocList, (DNODE *)psEarliestLoadControlEventRecord);
                    // send event and user c/b if this is a client
                    if(psClusterInstance->bIsServer==FALSE)
                    {
                        // set up report structure
                        eSE_DRLCInitialiseReportStructure(&sReportEvent,
                            psEarliestLoadControlEventRecord->sLoadControlEvent.u32IssuerId,
                            psEarliestLoadControlEventRecord->sLoadControlEvent.u8CriticalityLevel,
                            psEarliestLoadControlEventRecord->sLoadControlEvent.u8EventControl);
                        sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_HAS_BEEN_SUPERSEDED;
                        // check to see if the event is in the random period
                        if((eToEventList== E_SE_DRLC_EVENT_LIST_ACTIVE) && (boInEffectiveTimeExtendedEventOperation(psLoadControlEvent, psDRLC_Common->sDRLCCallBackMessage.u32CurrentTime)))
                        {
                            // consider this to be normal termination
                            sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_COMPLETED;
                        }
                        // send user event
                        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance, &sReportEvent);
                    }
                }
                else // added event is extinguished
                {
                    if(psClusterInstance->bIsServer==FALSE)
                    {
                        // overlap redundant event - don't do anything to the list - reject the event
                        eSE_DRLCInitialiseReportStructure(&sReportEvent,
                            psEarliestLoadControlEventRecord->sLoadControlEvent.u32IssuerId,
                            psEarliestLoadControlEventRecord->sLoadControlEvent.u8CriticalityLevel,
                            psEarliestLoadControlEventRecord->sLoadControlEvent.u8EventControl);
                        sReportEvent.u8EventStatus = E_SE_DRLC_LOAD_CONTROL_EVENT_COMMAND_REJECTED;
                        // send user event
                        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance, &sReportEvent);
                    }
                    return(E_ZCL_FAIL);
                }
            }
        }

        // get next
        psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetNext((DNODE *)psLoadControlEventRecord);
    }

    // no list option - we don't want to add it to a list
    if(eFromEventList== E_SE_DRLC_EVENT_LIST_NONE)
    {
        return(E_ZCL_SUCCESS);
    }

    // add event - this is now a simple add based on time with no event clashes to worry about
    vSimpleAddEventToScheduler(psDRLCCustomDataStructure, psLoadControlEventAddRecord, eFromEventList, eToEventList);

    // send event and user c/b if adding to active list (not if adding to scheduled list)
    if(eToEventList == E_SE_DRLC_EVENT_LIST_ACTIVE)
    {
        // Client - send RES
        if (psClusterInstance->bIsServer==FALSE)
        {
            // First Inform users that allows user can opt in/out at time of event active
            eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_ACTIVE, SE_DRLC_LOAD_CONTROL_EVENT, E_ZCL_SUCCESS);
            memcpy(&psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent, &psLoadControlEventAddRecord->sLoadControlEvent, sizeof(tsSE_DRLCLoadControlEvent));
            psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);


            // set up report structure
            eSE_DRLCInitialiseReportStructure(&sReportEvent,
                psLoadControlEventAdd->u32IssuerId,
                psLoadControlEventAdd->u8CriticalityLevel,
                psLoadControlEventAdd->u8EventControl);
            sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_STARTED;

            /* Should account from time of event active only */
            psLoadControlEventAddRecord->u8EventOptionChangeCount = 0;

            if(psLoadControlEventAddRecord->eEventOption == E_SE_DRLC_EVENT_USER_OPT_OUT)
            {
                sReportEvent.u8EventStatus = E_SE_DRLC_USER_CHOSEN_OPT_OUT;
            }

            eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&sReportEvent);
        }
        else
        {
#ifdef DRLC_SEND_LCE_AGAIN_AT_ACTIVE_TIME
            // Server - send the active event again for the benefit of servers which don't have a clock source
            psDRLC_Common->sDRLCCustomCallBackEvent.u8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
            eSE_DRLCLoadControlEventSend(
                psEndPointDefinition->u8EndPointNumber,
                psLoadControlEventAddRecord->u8DestinationEndPointId,
                &psLoadControlEventAddRecord->sDestinationAddress,
                &psLoadControlEventAddRecord->sLoadControlEvent,
                TRUE,
                &psDRLC_Common->sDRLCCustomCallBackEvent.u8TransactionSequenceNumber);
#endif
        }
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       bCheckForEventTimeOverlap
 **
 ** DESCRIPTION:
 ** checks to see if table times overlap
 **
 ** PARAMETERS:                          Name                                   Usage
 ** tsSE_DRLCLoadControlEventRecord      *psLoadControlEventAddRecord           table data
 ** tsSE_DRLCLoadControlEventRecord      *psLoadControlEventReferenceRecord     table data
 **
 ** RETURN:
 ** true/false
 **
 ****************************************************************************/

PRIVATE  bool_t bCheckForEventTimeOverlap(
                tsSE_DRLCLoadControlEventRecord   *psLoadControlEventAddRecord,
                tsSE_DRLCLoadControlEventRecord   *psLoadControlEventReferenceRecord)
{
    tsSE_DRLCLoadControlEvent    *psLoadControlEventRecordEarly;
    tsSE_DRLCLoadControlEvent    *psLoadControlEventRecordLate;

    uint32 u32earlyTime;
//    uint32 u32lateTime;

    tsSE_DRLCLoadControlEvent   *psLoadControlEventAdd = &psLoadControlEventAddRecord->sLoadControlEvent;
    tsSE_DRLCLoadControlEvent   *psLoadControlEventReference = &psLoadControlEventReferenceRecord->sLoadControlEvent;

    // find earliest start event
    psLoadControlEventRecordEarly = psLoadControlEventReference;
    psLoadControlEventRecordLate = psLoadControlEventAdd;


    if(psLoadControlEventAdd->u32StartTime <= psLoadControlEventReference->u32StartTime)
    {
        psLoadControlEventRecordEarly = psLoadControlEventAdd;
        psLoadControlEventRecordLate = psLoadControlEventReference;
    }

    u32earlyTime = psLoadControlEventRecordEarly->u32StartTime + psLoadControlEventRecordEarly->u16DurationInMinutes*60;
   // u32lateTime = psLoadControlEventRecordLate->u32StartTime + psLoadControlEventRecordLate->u16DurationInMinutes*60;

    if(psLoadControlEventRecordEarly->u32StartTime == psLoadControlEventRecordLate->u32StartTime)
    {
        return TRUE;
    }

    // allows for event ending the same time as other starts
    if(u32earlyTime > psLoadControlEventRecordLate->u32StartTime)
    {
        return TRUE;
    }

    return FALSE;

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       bGetEarliestStartTime
 **
 ** DESCRIPTION:
 ** indicates earliest event
 **
 ** PARAMETERS:                          Name                           Usage
 ** tsSE_DRLCLoadControlEventRecord      *psLoadControlEventAdd         table 1
 ** tsSE_DRLCLoadControlEventRecord      *psLoadControlEvent            table 2
 ** tsSE_DRLCLoadControlEventRecord     **ppsEarliestLoadControlEvent   earliest
 ** tsSE_DRLCLoadControlEventRecord     **ppsLatestLoadControlEvent     latest
 **
 ** RETURN:
 ** true/false
 **
 ****************************************************************************/

PRIVATE  bool_t bGetEarliestStartTime(
                tsSE_DRLCLoadControlEventRecord   *psLoadControlEventAddRecord,
                tsSE_DRLCLoadControlEventRecord   *psLoadControlEventRecord,
                tsSE_DRLCLoadControlEventRecord   **ppsEarliestLoadControlEventRecord,
                tsSE_DRLCLoadControlEventRecord   **ppsLatestLoadControlEventRecord)
{
    // preset
    *ppsLatestLoadControlEventRecord = psLoadControlEventAddRecord;
    *ppsEarliestLoadControlEventRecord = psLoadControlEventRecord;

    if(psLoadControlEventAddRecord->sLoadControlEvent.u32StartTime == psLoadControlEventRecord->sLoadControlEvent.u32StartTime)
    {
        if(psLoadControlEventRecord->sLoadControlEvent.u32IssuerId > psLoadControlEventAddRecord->sLoadControlEvent.u32IssuerId)
        {
            *ppsLatestLoadControlEventRecord = psLoadControlEventRecord;
            *ppsEarliestLoadControlEventRecord = psLoadControlEventAddRecord;
        }

    }
    else
    {
        if(psLoadControlEventRecord->sLoadControlEvent.u32StartTime > psLoadControlEventAddRecord->sLoadControlEvent.u32StartTime)
        {
            *ppsLatestLoadControlEventRecord = psLoadControlEventRecord;
            *ppsEarliestLoadControlEventRecord = psLoadControlEventAddRecord;
        }
    }

    // return flag to determine if the event we are trying to add is the later event
    if(*ppsEarliestLoadControlEventRecord != psLoadControlEventAddRecord)
    {
        return FALSE;
    }

    return TRUE;

}

/****************************************************************************
 **
 ** NAME:       boSearchForExisting
 **
 ** DESCRIPTION:
 ** Search helper function
 **
 ** PARAMETERS:             			Name              Usage
 ** void                    			*pvSearchParam    value to search on
 ** void                   *psNodeUnderTest  present node
 **
 ** RETURN:
 ** T/F - Success, -1 Fail
 **
 ****************************************************************************/

PRIVATE  bool_t boSearchForExisting(void *pvSearchParam, void *psNodeUnderTest)
{
    // search for the insert point
    if(
        (((tsSE_DRLCLoadControlEventRecord *)psNodeUnderTest)->sLoadControlEvent.u32IssuerId) == *((uint32 *)pvSearchParam)
    )
    {
        return TRUE;
    }

    return FALSE;
}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCFindDRLCCluster
 **
 ** DESCRIPTION:
 ** Search helper function
 **
 ** PARAMETERS:                     Name                            Usage
 ** uint8                           u8SourceEndPointId              Source EP Id
 ** bool_t                          bIsServer                       Is server
 ** tsZCL_EndPointDefinition        **ppsEndPointDefinition         EP
 ** tsZCL_ClusterInstance           **ppsClusterInstance            Cluster
 ** tsSE_DRLCCustomDataStructure    **ppsDRLCCustomDataStructure    event data
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCFindDRLCCluster(
                                uint8                           u8SourceEndPointId,
                                bool_t                          bIsServer,
                                tsZCL_EndPointDefinition        **ppsEndPointDefinition,
                                tsZCL_ClusterInstance           **ppsClusterInstance,
                                tsSE_DRLCCustomDataStructure    **ppsDRLCCustomDataStructure)
{
    // check EP is registered and cluster is present in the send device
    if(eZCL_SearchForEPentry(u8SourceEndPointId, ppsEndPointDefinition) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    if(eZCL_SearchForClusterEntry(u8SourceEndPointId, SE_CLUSTER_ID_DEMAND_RESPONSE_AND_LOAD_CONTROL, bIsServer, ppsClusterInstance) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
    }

    // initialise custom data pointer
    *ppsDRLCCustomDataStructure = (tsSE_DRLCCustomDataStructure *)(*ppsClusterInstance)->pvEndPointCustomStructPtr;
    if(*ppsDRLCCustomDataStructure==NULL)
    {
        return(E_ZCL_ERR_CUSTOM_DATA_NULL);
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       psCheckToSeeIfEventExists
 **
 ** DESCRIPTION:
 ** Select correct list head by checking system time
 **
 ** PARAMETERS:                     Name                            Usage
 ** tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure      list structure
 ** uint32                          u32IssuerId                     issuer Id
 ** teSE_DRLCEventList              *peEventList                    list event resides
 **
 ** RETURN:
 ** tsSE_DRLCLoadControlEventRecord
 **
 ****************************************************************************/

PUBLIC  tsSE_DRLCLoadControlEventRecord * psCheckToSeeIfEventExists(
                                    tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure,
                                    uint32                          u32IssuerId,
                                    teSE_DRLCEventList              *peEventList)
{
    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;
    DLIST *plEventList;

    // check each list to find the event
    psLoadControlEventRecord = psGetListHead(psDRLCCustomDataStructure, E_SE_DRLC_EVENT_LIST_SCHEDULED, &plEventList);
    // check to see if the entry exists - search through alloc list based on u32IssuerId
    psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTsearchForward((DNODE *)psLoadControlEventRecord, boSearchForExisting, (void *)&u32IssuerId);

    if(psLoadControlEventRecord!=NULL)
    {
        *peEventList = E_SE_DRLC_EVENT_LIST_SCHEDULED;
        return(psLoadControlEventRecord);
    }

    psLoadControlEventRecord = psGetListHead(psDRLCCustomDataStructure, E_SE_DRLC_EVENT_LIST_ACTIVE, &plEventList);
    // check to see if the entry exists - search through alloc list based on u32IssuerId
    psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTsearchForward((DNODE *)psLoadControlEventRecord, boSearchForExisting, (void *)&u32IssuerId);

    if(psLoadControlEventRecord!=NULL)
    {
        *peEventList = E_SE_DRLC_EVENT_LIST_ACTIVE;
        return(psLoadControlEventRecord);
    }

    psLoadControlEventRecord = psGetListHead(psDRLCCustomDataStructure, E_SE_DRLC_EVENT_LIST_CANCELLED, &plEventList);
    // check to see if the entry exists - search through alloc list based on u32IssuerId
    psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTsearchForward((DNODE *)psLoadControlEventRecord, boSearchForExisting, (void *)&u32IssuerId);

    if(psLoadControlEventRecord!=NULL)
    {
        *peEventList = E_SE_DRLC_EVENT_LIST_CANCELLED;
        return(psLoadControlEventRecord);
    }

    return(NULL);

}

/****************************************************************************
 **
 ** NAME:       psGetListHead
 **
 ** DESCRIPTION:
 ** Get Head of a specified list
 **
 ** PARAMETERS:                   Name                        Usage
 ** tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure   list structure
 ** teSE_DRLCEventList            eEventList                  list to get
 ** DLIST                        **pplEventList               ptr to list
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  tsSE_DRLCLoadControlEventRecord * psGetListHead(
                                    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure,
                                    teSE_DRLCEventList            eEventList,
                                    DLIST                        **pplEventList)
{
    tsSE_DRLCLoadControlEventRecord *psHeadLoadControlEventRecord = 0;

    switch(eEventList)
    {
        case(E_SE_DRLC_EVENT_LIST_SCHEDULED):
        {
            psHeadLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead( &psDRLCCustomDataStructure->lLoadControlEventScheduledList);
            *pplEventList = &psDRLCCustomDataStructure->lLoadControlEventScheduledList;
            break;
        }
        case(E_SE_DRLC_EVENT_LIST_ACTIVE):
        {
            psHeadLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead( &psDRLCCustomDataStructure->lLoadControlEventActiveList);
            *pplEventList = &psDRLCCustomDataStructure->lLoadControlEventActiveList;
            break;
        }
        case(E_SE_DRLC_EVENT_LIST_CANCELLED):
        {
            psHeadLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead( &psDRLCCustomDataStructure->lLoadControlEventCancelledList);
            *pplEventList = &psDRLCCustomDataStructure->lLoadControlEventCancelledList;
            break;
        }
        case(E_SE_DRLC_EVENT_LIST_DEALLOCATED):
        {
            psHeadLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead( &psDRLCCustomDataStructure->lLoadControlEventDeAllocList);
            *pplEventList = &psDRLCCustomDataStructure->lLoadControlEventDeAllocList;
            break;
        }
        default:
        {
            break;
        }
    }
    return psHeadLoadControlEventRecord;
}

/****************************************************************************
 **
 ** NAME:       eAddToSchedulerList
 **
 ** DESCRIPTION:
 ** Adds an event to the correct list
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_EndPointDefinition        *psEndPointDefinition       EP structure
 ** tsZCL_ClusterInstance           *psClusterInstance          Cluster structure
 ** tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure  list access
 ** tsLoadControlEvent              *psLoadControlEvent         table data
 ** teSE_DRLCEventList              eFromEventList              source list
 ** teSE_DRLCEventList              eToEventList                destination list
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCAddToSchedulerList(
                                 tsZCL_EndPointDefinition           *psEndPointDefinition,
                                 tsZCL_ClusterInstance              *psClusterInstance,
                                 tsSE_DRLCCustomDataStructure       *psDRLCCustomDataStructure,
                                 tsSE_DRLCLoadControlEventRecord    *psLoadControlEventRecord,
                                 teSE_DRLCEventList                  eFromEventList,
                                 teSE_DRLCEventList                  eToEventList)
{

    //  need to remove overlap check on all lists except dealloc
    if(eToEventList!=E_SE_DRLC_EVENT_LIST_DEALLOCATED)
    {
        return eSE_DRLCOverlapAddEventToScheduler(psEndPointDefinition,
                                                          psClusterInstance,
                                                          psLoadControlEventRecord,
                                                          psDRLCCustomDataStructure,
                                                          eFromEventList,
                                                          eToEventList);
    }

    // simple list add in time order
    vSimpleAddEventToScheduler(psDRLCCustomDataStructure, psLoadControlEventRecord, eFromEventList, eToEventList);
    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       vSimpleAddEventToScheduler
 **
 ** DESCRIPTION:
 ** Adds an event to the correct list
 **
 ** PARAMETERS:                 Name            Usage
 ** tsSE_DRLCCustomDataStructure  *psDRLCCustomDataStructure  list access
 ** tsLoadControlEvent            *psLoadControlEventRecord   table data
 ** teSE_DRLCEventList             eFromEventList             source list
 ** teSE_DRLCEventList             eToEventList               destination list
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  void vSimpleAddEventToScheduler(
                tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure,
                tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord,
                teSE_DRLCEventList               eFromEventList,
                teSE_DRLCEventList               eToEventList)
{
    tsSE_DRLCLoadControlEventRecord *psHeadLoadControlEventRecord;
    DLIST *plToEventList;
    DLIST *plFromEventList;

    // no list option
    if(eFromEventList== E_SE_DRLC_EVENT_LIST_NONE)
    {
        return;
    }

    psHeadLoadControlEventRecord = psGetListHead(psDRLCCustomDataStructure, eToEventList, &plToEventList);

    // add in time order
    while((psHeadLoadControlEventRecord!=NULL) &&
          (psHeadLoadControlEventRecord->sLoadControlEvent.u32StartTime < psLoadControlEventRecord->sLoadControlEvent.u32StartTime)
    )
    {
        // get next
        psHeadLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetNext((DNODE *)psHeadLoadControlEventRecord);
    }

    // remove from source list
    psGetListHead(psDRLCCustomDataStructure, eFromEventList, &plFromEventList);
    psDLISTremove(plFromEventList, (DNODE *)psLoadControlEventRecord);

    if(psHeadLoadControlEventRecord==NULL)
    {
        // add to tail - this is same as adding to head when the list is empty
        vDLISTaddToTail(plToEventList, (DNODE *)psLoadControlEventRecord);
     }
     else
     {
        vDLISTinsertBefore(plToEventList, (DNODE *)psHeadLoadControlEventRecord, (DNODE *)psLoadControlEventRecord);
     }

}

/****************************************************************************
 **
 ** NAME:       vRemoveFromSchedulerList
 **
 ** DESCRIPTION:
 ** Removes a record from the correct list
 **
 ** PARAMETERS:                   Name                        Usage
 ** tsSE_DRLCCustomDataStructure  *psDRLCCustomDataStructure  list access
 ** tsLoadControlEvent            *psLoadControlEventRecord   table data
 ** teSE_DRLCEventList             eEventList                 source list
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  void vRemoveFromSchedulerList(
                   tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure,
                   tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord,
                   teSE_DRLCEventList               eEventList)
{

    switch(eEventList)
    {
        case(E_SE_DRLC_EVENT_LIST_SCHEDULED):
        {
            psDLISTremove( &psDRLCCustomDataStructure->lLoadControlEventScheduledList, (DNODE *)psLoadControlEventRecord);
            break;
        }
        case(E_SE_DRLC_EVENT_LIST_ACTIVE):
        {
            psDLISTremove( &psDRLCCustomDataStructure->lLoadControlEventActiveList, (DNODE *)psLoadControlEventRecord);
            break;
        }
        case(E_SE_DRLC_EVENT_LIST_CANCELLED):
        {
            psDLISTremove( &psDRLCCustomDataStructure->lLoadControlEventCancelledList, (DNODE *)psLoadControlEventRecord);
            break;
        }
        default:
        {
            break;
        }
    }

    vDLISTaddToTail(&psDRLCCustomDataStructure->lLoadControlEventDeAllocList, (DNODE *)(psLoadControlEventRecord));
}


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

