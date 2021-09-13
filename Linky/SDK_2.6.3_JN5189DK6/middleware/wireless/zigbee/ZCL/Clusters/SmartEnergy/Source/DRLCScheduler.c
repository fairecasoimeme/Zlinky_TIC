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
 * DESCRIPTION:        DRLC Event scheduler
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

PRIVATE void vSE_DRLCTimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eSE_DRLCRegisterTimeServer
 **
 ** DESCRIPTION:
 ** Registers DRLC Time Server with ZCL
 **
 ** PARAMETERS:                         Name                          Usage
 ** none
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCRegisterTimeServer(void)
{
    static bool_t bTimerRegistered = FALSE;
#ifndef OTA_UNIT_TEST_FRAMEWORK
    if(!bTimerRegistered)
#endif
    {
        // add timer click function to ZCL
        if(eZCL_TimerRegister(E_ZCL_TIMER_CLICK, 0, vSE_DRLCTimerClickCallback)!= E_ZCL_SUCCESS)
            return(E_ZCL_FAIL);

        bTimerRegistered = TRUE;
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       vSE_DRLCTimerClickCallback
 **
 ** DESCRIPTION:
 ** Timer Callback registered with the ZCL
 **
 ** PARAMETERS:                 Name               Usage
 ** tsZCL_CallBackEvent        *psCallBackEvent    Timer Server Callback
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  void vSE_DRLCTimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent)
{
    int i;
    uint8 u8numberOfendpoints;

    u8numberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();

    // find DRLC clusters on each EP - if any
    for(i=0; i<u8numberOfendpoints; i++)
    {
        // deliver time to any EP-server/client
        eSE_DRLCSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i), TRUE, psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
        eSE_DRLCSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i), FALSE, psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
    }

}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCSchedulerUpdate
 **
 ** DESCRIPTION:
 ** Updates the DRLC event Table - based on current time
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32UTCtime                  Current time
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCSchedulerUpdate(
                            uint8   u8SourceEndPointId,
                            bool_t  bIsServer,
                            uint32  u32UTCtime)
{

    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;

    uint8  u8FindDRLCClusterReturn;
    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

      tsDRLC_Common *psDRLC_Common;

    // error check via EP number
      u8FindDRLCClusterReturn = eSE_DRLCFindDRLCCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psDRLCCustomDataStructure);
    if(u8FindDRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindDRLCClusterReturn;
    }

    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psDRLC_Common->sDRLCCustomCallBackEvent, 0, 0, psEndPointDefinition->u8EndPointNumber);

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // get first resource from active list and check for expired entries
    psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead(&psDRLCCustomDataStructure->lLoadControlEventActiveList);
    // search
    while(psLoadControlEventRecord != NULL)
    {
        // have to full search the list as the random start/stop times can change results
        if(boEffectiveExpiredTimeCheck(psLoadControlEventRecord, psEndPointDefinition, psClusterInstance, u32UTCtime))
        {
            // remove it from active list
            psDLISTremove(&psDRLCCustomDataStructure->lLoadControlEventActiveList, (DNODE *)psLoadControlEventRecord);
            // add to free list
            vDLISTaddToTail(&psDRLCCustomDataStructure->lLoadControlEventDeAllocList, (DNODE *)psLoadControlEventRecord);
            // set the correct expiry
            if(psLoadControlEventRecord->eEventOption == E_SE_DRLC_EVENT_USER_OPT_IN)
            {
                psLoadControlEventRecord->sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_COMPLETED;
                if(psLoadControlEventRecord->u8EventOptionChangeCount)
                {
                    psLoadControlEventRecord->sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_PARTIALLY_COMPLETED_WITH_USER_OPT_IN;
                }
            }
            else // E_SE_DRLC_EVENT_USER_OPT_OUT
            {
                psLoadControlEventRecord->sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_COMPLETED_NO_USER_PARTICIPATION;
                if(psLoadControlEventRecord->u8EventOptionChangeCount)
                {
                    psLoadControlEventRecord->sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_PARTIALLY_COMPLETED_WITH_USER_OPT_OUT;
                }
            }
            // send event
            if(bIsServer==FALSE)
            {
                eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&psLoadControlEventRecord->sReportEvent);
            }
            // inform users -  send LCE
            eDRLC_SetCallBackMessage(psDRLC_Common, E_SE_DRLC_EVENT_EXPIRED, SE_DRLC_LOAD_CONTROL_EVENT,  E_ZCL_SUCCESS);
            memcpy(&psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent, &psLoadControlEventRecord->sLoadControlEvent, sizeof(tsSE_DRLCLoadControlEvent));
            psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
        }

        // get next resource from active list
        psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetNext((DNODE *)&psLoadControlEventRecord->dllrlcNode);

    }

    // get first resource from scheduled list
    psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead(&psDRLCCustomDataStructure->lLoadControlEventScheduledList);
    // search
    while(psLoadControlEventRecord != NULL)
    {
        // have to full search the list as the random start/stop times can change results
        if(boInEffectiveActiveTimeCheck(psLoadControlEventRecord, psEndPointDefinition, psClusterInstance, u32UTCtime))
        {
            // if cancel set - must be deferred if the scheduler is dealing with it
            if(psLoadControlEventRecord->bDeferredCancel==TRUE)
            {
                // check for clashes with anything in the active list list without adding then add to the cancelled list
                eSE_DRLCAddToSchedulerList(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, psLoadControlEventRecord, E_SE_DRLC_EVENT_LIST_NONE, E_SE_DRLC_EVENT_LIST_ACTIVE);
                eSE_DRLCAddToSchedulerList(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, psLoadControlEventRecord, E_SE_DRLC_EVENT_LIST_SCHEDULED, E_SE_DRLC_EVENT_LIST_CANCELLED);
            }
            else
            {
                // check for clashes with anything in the cancelled list list without adding then add to the active list
                eSE_DRLCAddToSchedulerList(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, psLoadControlEventRecord, E_SE_DRLC_EVENT_LIST_NONE, E_SE_DRLC_EVENT_LIST_CANCELLED);
                eSE_DRLCAddToSchedulerList(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, psLoadControlEventRecord, E_SE_DRLC_EVENT_LIST_SCHEDULED, E_SE_DRLC_EVENT_LIST_ACTIVE);
            }
        }

        // get next resource from scheduled list
        psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetNext((DNODE *)&psLoadControlEventRecord->dllrlcNode);
    }

    // get first resource from cancel list
    psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead(&psDRLCCustomDataStructure->lLoadControlEventCancelledList);
    // search
    while(psLoadControlEventRecord != NULL)
    {
        // have to full search the list as the random start/stop times can change results
        if(boCancelTimeCheck(psLoadControlEventRecord, psEndPointDefinition, psClusterInstance, u32UTCtime))
        {
            // move back to dealloc list
            psDLISTremove( &psDRLCCustomDataStructure->lLoadControlEventCancelledList, (DNODE *)psLoadControlEventRecord);
            // add to tail
            vDLISTaddToTail(&psDRLCCustomDataStructure->lLoadControlEventDeAllocList, (DNODE *)psLoadControlEventRecord);

            if(psClusterInstance->bIsServer==FALSE)
            {
                // send event
                psLoadControlEventRecord->sReportEvent.u8EventStatus = E_SE_DRLC_EVENT_HAS_BEEN_CANCELLED;
                eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&psLoadControlEventRecord->sReportEvent);
            }

            // inform users
            memcpy(&psDRLC_Common->sDRLCCallBackMessage.uMessage.sLoadControlEvent, &psLoadControlEventRecord->sLoadControlEvent, sizeof(tsSE_DRLCLoadControlEvent));
            eDRLC_SetCallBackMessage(psDRLC_Common,  E_SE_DRLC_EVENT_CANCELLED, SE_DRLC_LOAD_CONTROL_EVENT, E_ZCL_SUCCESS);
            psEndPointDefinition->pCallBackFunctions(&psDRLC_Common->sDRLCCustomCallBackEvent);
        }

        // get next resource from cancelled list
        psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetNext((DNODE *)&psLoadControlEventRecord->dllrlcNode);
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

