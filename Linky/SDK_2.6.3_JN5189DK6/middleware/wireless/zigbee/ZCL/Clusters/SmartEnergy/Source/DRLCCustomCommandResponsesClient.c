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
 * DRLC client command handlers
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
PRIVATE void vSE_DRLCCancelAllLoadControlEvent(
                            tsZCL_EndPointDefinition           *psEndPointDefinition,
                            tsZCL_ClusterInstance              *psClusterInstance,
                            teSE_DRLCEventList                  eEventList);

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
 ** NAME:       vSE_DRLCHandleLoadControlEvent
 **
 ** DESCRIPTION:
 ** Handles Load Control Event Command
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

PUBLIC  void vSE_DRLCHandleLoadControlEvent(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    uint8 u8TransactionSequenceNumber;

    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;

    uint8 u8DRLCClusterReturn;
    tsSE_DRLCReportEvent sReportEvent;

    uint8 u8UtilityEnrolmentGroup;
    uint16 u16DeviceClass;

    tsDRLC_Common *psDRLC_Common;

    teSE_DRLCEventList eTempEventList;

    // set up report structure
    memset(&sReportEvent, 0, sizeof(tsSE_DRLCReportEvent));

    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;
    // initialise custom data pointer
    psDRLCCustomDataStructure = (tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // get load control information
    u8DRLCClusterReturn = eSE_DRLCLoadControlEventReceive(pZPSevent, psEndPointDefinition, &psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent, &u8TransactionSequenceNumber);

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psDRLC_Common->sDRLCCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT, u8DRLCClusterReturn);

    // carry on
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        // assume we don't send an event for this - the data isn't valid to be able act on
        // call user
        memset(&psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent, 0, sizeof(tsSE_DRLCLoadControlEvent));
        psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
        // release EP mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    eZCL_GetLocalAttributeValue(E_CLD_DRLC_UTILITY_ENROLMENT_GROUP, FALSE,TRUE, psEndPointDefinition, psClusterInstance, &u8UtilityEnrolmentGroup);

    if((u8UtilityEnrolmentGroup != psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u8UtilityEnrolmentGroup) &&
       (psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u8UtilityEnrolmentGroup != 0))
    {
        // assume we don't send an event for this
        // release EP mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // check device ID bitmap overlap - ignore
    eZCL_GetLocalAttributeValue(E_CLD_DRLC_DEVICE_CLASS_VALUE, FALSE, TRUE,psEndPointDefinition, psClusterInstance, &u16DeviceClass);

    // If device class is set to a reserved value, ignore the event - SE test 10.18.1
    if (psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u16DeviceClass & E_SE_DRLC_DEVICE_CLASS_RESERVED_BITS_MASK)
    {
        // assume we don't send an event for this
        // release EP mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // mask u16DeviceClass to just ones relevant to us - makes overlap checks less messy
    psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u16DeviceClass &= u16DeviceClass;
    // no bits set, ignore
    if(psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u16DeviceClass == 0)
    {
        // assume we don't send an event for this
        // release EP mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // If criticallity is set to a reserved value, ignore the event - SE test 10.19.1
    if (psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u8CriticalityLevel >= E_SE_DRLC_FIRST_RESERVED_CRITICALITY)
    {
        // assume we don't send an event for this
        // release EP mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }


    // initialise local event subfields
    eSE_DRLCInitialiseReportStructure(&sReportEvent,
        psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32IssuerId,
        psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u8CriticalityLevel,
        psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u8EventControl);

    // check validity
    // Adjust start time of now to the current time.
    if (psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32StartTime  == 0)
    {
        psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32StartTime = psDRLC_Common->sDRLCCallBackMessage.u32CurrentTime;
    }



    // has event arrived in time to execute at all ?
    if(psDRLC_Common->sDRLCCallBackMessage.u32CurrentTime > (psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32StartTime + psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u16DurationInMinutes*60))
    {
        // send event
        sReportEvent.u8EventStatus = E_SE_DRLC_REJECTED_EVENT_RECEIVED_AFTER_IT_HAD_EXPIRED;
        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&sReportEvent);
        // call user
        eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT, E_ZCL_FAIL);
        psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
        // release EP mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

        // Is the duration too long (SE test 10.24)
    if(psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u16DurationInMinutes > E_SE_DRLC_MAX_EVENT_DURATION)
    {
        // send event
        sReportEvent.u8EventStatus = E_SE_DRLC_LOAD_CONTROL_EVENT_COMMAND_REJECTED;
        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&sReportEvent);
        // call user
        eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT, E_ZCL_FAIL);
        psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
        // release EP mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // is it late but can still run ?
    if((psDRLC_Common->sDRLCCallBackMessage.u32CurrentTime >= psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32StartTime) &&
       (psDRLC_Common->sDRLCCallBackMessage.u32CurrentTime < (psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32StartTime +
                                                              psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u16DurationInMinutes*60))
    )
    {
        // modify the event start time and duration
        psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32StartTime = psDRLC_Common->sDRLCCallBackMessage.u32CurrentTime;
        psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u16DurationInMinutes -= ((uint16)(psDRLC_Common->sDRLCCallBackMessage.u32CurrentTime - psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32StartTime))/60;

        // carry on as normal
    }

    // call user and send RES to inform of incoming command
    // Do this before adding to list as the adding to list may overwrite the event to send its own
    // RES such as superseded.  Also Superseded SE test spec test expects the command RES before
    // the superseded RES.
    sReportEvent.u8EventStatus = E_SE_DRLC_LOAD_CONTROL_EVENT_COMMAND_RECEIVED;
    psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);

    // SE test spec r 15 (094980) test 10.17 (Duplicate event IDs) expects an LCE with a
    // duplicate issuer ID to be siliently discarded without a Command RES.
    if (psCheckToSeeIfEventExists(psDRLCCustomDataStructure,
        psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent.u32IssuerId,
        &eTempEventList) == NULL)
    {
        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance, &sReportEvent);
    }

    // add LCE to list
    u8DRLCClusterReturn = eSE_DRLCAddLoadControlEventNoMutex(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, &psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent);

    // Don't send RES for duplicate - see note on SE test spec above
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS && u8DRLCClusterReturn != E_SE_DRLC_DUPLICATE_EXISTS )
    {
        sReportEvent.u8EventStatus = E_SE_DRLC_LOAD_CONTROL_EVENT_COMMAND_REJECTED;
        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance, &sReportEvent);
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}

/****************************************************************************
 **
 ** NAME:       vSE_DRLCHandleLoadControlEventCancel
 **
 ** DESCRIPTION:
 ** Handles DRLC CLuster custom commands
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

PUBLIC  void vSE_DRLCHandleLoadControlEventCancel(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    uint8 u8TransactionSequenceNumber;

    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;

    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;
    teSE_DRLCEventList eEventList;

    uint8 u8DRLCClusterReturn;
    tsSE_DRLCReportEvent sReportEvent;

    uint8 u8UtilityEnrolmentGroup;
    uint16 u16DeviceClass;

    tsDRLC_Common *psDRLC_Common;

    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;
    // initialise custom data structure pointer
    psDRLCCustomDataStructure = (tsSE_DRLCCustomDataStructure *)(psClusterInstance->pvEndPointCustomStructPtr);

     // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // get cancel event
        u8DRLCClusterReturn = eSE_DRLCCancelLoadControlEventReceive(pZPSevent, psEndPointDefinition, &psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent, &u8TransactionSequenceNumber);
    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psDRLC_Common->sDRLCCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    // call user
    eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT_CANCEL, u8DRLCClusterReturn);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        // assume we don't send an event for this - the data isn't valid to be able act on
        // call user
        memset(&psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent, 0, sizeof(tsSE_DRLCCancelLoadControlEvent));
        psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    eZCL_GetLocalAttributeValue(E_CLD_DRLC_UTILITY_ENROLMENT_GROUP, FALSE,TRUE, psEndPointDefinition, psClusterInstance, &u8UtilityEnrolmentGroup);

    if((u8UtilityEnrolmentGroup != psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u8UtilityEnrolmentGroup) &&
       (psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u8UtilityEnrolmentGroup != 0))
    {
        // not for us
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // check device ID bitmap overlap - ignore
    eZCL_GetLocalAttributeValue(E_CLD_DRLC_DEVICE_CLASS_VALUE, FALSE,TRUE, psEndPointDefinition, psClusterInstance, &u16DeviceClass);

    // mask u16DeviceClass to just ones relevant to us - makes overlap checks less messy
    psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u16DeviceClass &= u16DeviceClass;
    // no bits set, ignore
    if(psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u16DeviceClass == 0)
    {
        // not for us
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    psLoadControlEventRecord = psCheckToSeeIfEventExists(psDRLCCustomDataStructure, psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u32IssuerId, &eEventList);
    // initialise local event subfields
    // Daniel:- Setting the Criticality level to 1 because it is present in all report event status messages, even those which
    // don't relate to a valid event being cancelled.  Zero is a reserved value and documented as "not currently used" in the SE spec.
    // Similarly, event control is set to zero (No randomisation) for want of a better value.
    // The Zigbee SE spec really needs a CCB to define a default value for these cases.
    eSE_DRLCInitialiseReportStructure(&sReportEvent, psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u32IssuerId, 0, 0);
    // return if error
    if(psLoadControlEventRecord==NULL)
    {
        sReportEvent.u8EventStatus = E_SE_DRLC_REJECTED_INVALID_CANCEL_COMMAND_UNDEFINED_EVENT;
        // send event
        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance, &sReportEvent);
        // call user
        eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT_CANCEL, E_SE_DRLC_EVENT_NOT_FOUND);
        psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // remove event from list
    u8DRLCClusterReturn = eSE_DRLCCancelLoadControlEventNoMutex(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, &psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent);
    // call user to inform of incoming command, whatever the result
    eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT_CANCEL, u8DRLCClusterReturn);
    psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);

    // check for deferred condition first
    if(u8DRLCClusterReturn == E_SE_DRLC_CANCEL_DEFERRED)
    {
        // the scheduler will take care of the events to the server and user c/b
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // then any error
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        sReportEvent.u8EventStatus = E_SE_DRLC_REJECTED_INVALID_CANCEL_COMMAND_DEFAULT;
        sReportEvent.u8CriticalityLevelApplied =  psLoadControlEventRecord->sLoadControlEvent.u8CriticalityLevel;
        // send event
        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&sReportEvent);

        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // call user to inform of successful cancel and item removed from list
    psDRLC_Common->sDRLCCallBackMessage.eEventType = E_SE_DRLC_EVENT_CANCELLED;
    psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u32IssuerId = psLoadControlEventRecord->sLoadControlEvent.u32IssuerId;
    psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u16DeviceClass = psLoadControlEventRecord->sLoadControlEvent.u16DeviceClass;
    psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u8UtilityEnrolmentGroup = psLoadControlEventRecord->sLoadControlEvent.u8UtilityEnrolmentGroup;
   // psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.eCancelControl = psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.eCancelControl;
    psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u32effectiveTime = u32ZCL_GetUTCTime();

    psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // send ok
    sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_HAS_BEEN_CANCELLED;
    sReportEvent.u8CriticalityLevelApplied =  psLoadControlEventRecord->sLoadControlEvent.u8CriticalityLevel;
    eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&sReportEvent);

}

/****************************************************************************
 **
 ** NAME:       vSE_DRLCHandleLoadControlEventCancelAll
 **
 ** DESCRIPTION:
 ** Handles DRLC CLuster custom commands
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

PUBLIC  void vSE_DRLCHandleLoadControlEventCancelAll(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    uint8 u8TransactionSequenceNumber;

    uint8 u8DRLCClusterReturn;

    tsDRLC_Common *psDRLC_Common;
    DLIST *plEventList;
    tsSE_DRLCReportEvent sReportEvent;
    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // get cancel all event
    u8DRLCClusterReturn = eSE_DRLCCancelAllLoadControlEventsReceive(pZPSevent, psEndPointDefinition, &(psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlAllEvent), &u8TransactionSequenceNumber);
    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psDRLC_Common->sDRLCCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT_CANCEL_ALL, u8DRLCClusterReturn);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        // assume we don't send an event for this - the data isn't valid to be able act on
        // call user
        memset(&psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlAllEvent, 0, sizeof(tsSE_DRLCCancelLoadControlAllEvent));
        psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);

        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // call user to inform of incoming command
    eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_COMMAND, SE_DRLC_LOAD_CONTROL_EVENT_CANCEL_ALL, E_ZCL_SUCCESS);
    psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);

    // no need to check enrolment group or device ID
    if((NULL == psGetListHead((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr, E_SE_DRLC_EVENT_LIST_ACTIVE, &plEventList))&&
       (NULL == psGetListHead((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr, E_SE_DRLC_EVENT_LIST_SCHEDULED, &plEventList))&&
       (NULL == psGetListHead((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr, E_SE_DRLC_EVENT_LIST_CANCELLED, &plEventList)))
    {
        eSE_DRLCInitialiseReportStructure(&sReportEvent,0, 1, 0);
        sReportEvent.u8EventStatus = E_SE_DRLC_REJECTED_INVALID_CANCEL_COMMAND_DEFAULT;
        // send event
        eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&sReportEvent);
    }
    else
    {
        // get head of each list - cancel each event
        vSE_DRLCCancelAllLoadControlEvent(psEndPointDefinition, psClusterInstance, E_SE_DRLC_EVENT_LIST_ACTIVE);
        vSE_DRLCCancelAllLoadControlEvent(psEndPointDefinition, psClusterInstance, E_SE_DRLC_EVENT_LIST_SCHEDULED);
        vSE_DRLCCancelAllLoadControlEvent(psEndPointDefinition, psClusterInstance, E_SE_DRLC_EVENT_LIST_CANCELLED);
    }
    // report is generated for each individually cancelled event

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


}

/****************************************************************************
 **
 ** NAME:       vSE_DRLCCancelAllLoadControlEvent
 **
 ** DESCRIPTION:
 ** cancel load control event
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_EndPointDefinition        *psEndPointDefinition       EP structure
 ** tsZCL_ClusterInstance           *psClusterInstance          Cluster structure
 ** teSE_DRLCEventList               eEventList                 src list
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/
PRIVATE  void vSE_DRLCCancelAllLoadControlEvent(
                            tsZCL_EndPointDefinition           *psEndPointDefinition,
                            tsZCL_ClusterInstance              *psClusterInstance,
                            teSE_DRLCEventList                  eEventList)
{

    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord, *psNextLoadControlEventRecord;
    DLIST *plEventList;
    teSE_DRLCCancelControl eCancelControl;

    tsDRLC_Common *psDRLC_Common;
    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;

    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;
    // initialise custom data pointer
    psDRLCCustomDataStructure = (tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;
    // get cancel mode from incoming message
    eCancelControl = psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlAllEvent.u8CancelControl;
    // get start of list
    psLoadControlEventRecord = psGetListHead(psDRLCCustomDataStructure, eEventList, &plEventList);

    while(psLoadControlEventRecord !=NULL)
    {
        // Need to get next now because when record is moved onto the cancel list, its next pointer may be adjusted.
        psNextLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetNext((DNODE *)&psLoadControlEventRecord->dllrlcNode);
        // check for lateness or nearly late
        if(eCancelControl == E_SE_DRLC_CANCEL_CONTROL_IMMEDIATE)
        {
            // remove immediately
            vSimpleAddEventToScheduler(psDRLCCustomDataStructure, psLoadControlEventRecord, eEventList, E_SE_DRLC_EVENT_LIST_DEALLOCATED);
            // send report event
            psLoadControlEventRecord->sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_HAS_BEEN_CANCELLED;
            // send user event
            eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance, &psLoadControlEventRecord->sReportEvent);
            // send user callback using the cancel load control structure - per cancel event
            psDRLC_Common->sDRLCCallBackMessage.eEventType = E_SE_DRLC_EVENT_CANCELLED;
            psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u32IssuerId = psLoadControlEventRecord->sLoadControlEvent.u32IssuerId;
            psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u16DeviceClass = psLoadControlEventRecord->sLoadControlEvent.u16DeviceClass;
            psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u8UtilityEnrolmentGroup = psLoadControlEventRecord->sLoadControlEvent.u8UtilityEnrolmentGroup;
            psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.eCancelControl = eCancelControl;
            psDRLC_Common->sDRLCCallBackMessage.uMessage.sCancelLoadControlEvent.u32effectiveTime = u32ZCL_GetUTCTime();
            psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
        }
        else
        {
            // write in the cancel time to the event and store it on the cancel list if it is already active
            psLoadControlEventRecord->u32EffectiveCancelTime = u32ZCL_GetUTCTime();
            psLoadControlEventRecord->eCancelControl = eCancelControl;
            psLoadControlEventRecord->bDeferredCancel = TRUE;
            // only move to cancelled list when active
            if(eEventList==E_SE_DRLC_EVENT_LIST_ACTIVE)
            {
                // remove and add to cancelled
                vSimpleAddEventToScheduler(psDRLCCustomDataStructure, psLoadControlEventRecord, E_SE_DRLC_EVENT_LIST_ACTIVE, E_SE_DRLC_EVENT_LIST_CANCELLED);
            }

            // report event and c/b will be sent via the scheduler in this case
        }

        // get next
        psLoadControlEventRecord = psNextLoadControlEventRecord;
    }
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
