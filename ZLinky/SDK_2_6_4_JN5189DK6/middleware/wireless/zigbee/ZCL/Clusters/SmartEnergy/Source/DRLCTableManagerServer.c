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
 * DRLC table manager for server
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

PRIVATE void vRemoveAllEventsFromList(tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure,
                                      teSE_DRLCEventList               eEventList);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eSE_DRLCAddLoadControlEvent
 **
 ** DESCRIPTION:
 ** Adds a LCE event to the database
 **
 ** PARAMETERS:                 Name                            Usage
 ** uint8                       u8SourceEndPointId              Source EP Id
 ** uint8                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address               *psDestinationAddress           Destination Address
 ** tsSE_DRLCLoadControlEvent   *psLoadControlEvent             LoadControlEvent struct
 ** uint8                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCAddLoadControlEvent(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    tsSE_DRLCLoadControlEvent   *psLoadControlEvent,
                    uint8                       *pu8TransactionSequenceNumber)
{

    uint8 u8DRLCClusterReturn;

    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;
    bool_t    bIsDRLCEventStartNowEntry = FALSE;

    if (!bZCL_GetTimeHasBeenSynchronised())
    {
        return E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
    }

    // paramter checks
    if((psDestinationAddress== NULL) || (psLoadControlEvent== NULL) || (pu8TransactionSequenceNumber== NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

#ifndef DRLC_TEST_HARNESS
    if (psLoadControlEvent->u16DeviceClass & E_SE_DRLC_DEVICE_CLASS_RESERVED_BITS_MASK)
    {
        return(E_SE_DRLC_BAD_DEVICE_CLASS);
    }

    if (psLoadControlEvent->u8CriticalityLevel >= E_SE_DRLC_FIRST_RESERVED_CRITICALITY)
    {
        return(E_SE_DRLC_BAD_CRITICALITY_LEVEL);
    }

    if (psLoadControlEvent->u16DurationInMinutes > E_SE_DRLC_MAX_EVENT_DURATION)
    {
        return(E_SE_DRLC_DURATION_TOO_LONG);
    }
#endif

    // error check via EP number
    u8DRLCClusterReturn = eSE_DRLCFindDRLCCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psDRLCCustomDataStructure);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8DRLCClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // Adjust start time of now to the current time.
    if (psLoadControlEvent->u32StartTime == 0)
    {
        psLoadControlEvent->u32StartTime = u32ZCL_GetUTCTime();
        bIsDRLCEventStartNowEntry = TRUE;
    }

    u8DRLCClusterReturn = eSE_DRLCAddLoadControlEventNoMutex(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, psLoadControlEvent);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8DRLCClusterReturn;
    }

    /* check is the entry is start now, set time to NOW */
    if(bIsDRLCEventStartNowEntry)
    {
        psLoadControlEvent->u32StartTime = 0;
    }

    // send event automatically
    *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
    return eSE_DRLCLoadControlEventSend(u8SourceEndPointId, u8DestinationEndPointId, psDestinationAddress, psLoadControlEvent, FALSE, pu8TransactionSequenceNumber);

}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCGetLoadControlEvent
 **
 ** DESCRIPTION:
 ** Gats a LCE specified by table index
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      u8tableIndex                Table Index
 ** teSE_DRLCEventList          eEventList                  List Type
 ** tsSE_DRLCLoadControlEvent   ppsLoadControlEvent         replace table if exists
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCGetLoadControlEvent(
            uint8                       u8SourceEndPointId,
            uint8                       u8tableIndex,
            teSE_DRLCEventList          eEventList,
            tsSE_DRLCLoadControlEvent   **ppsLoadControlEvent
)
{
    int i;

    uint8 u8DRLCClusterReturn;

    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;
    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;

    DLIST *plEventList;

    if(ppsLoadControlEvent == NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    // error check via EP number
    u8DRLCClusterReturn = eSE_DRLCFindDRLCCluster(u8SourceEndPointId, FALSE, &psEndPointDefinition, &psClusterInstance, &psDRLCCustomDataStructure);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8DRLCClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    psLoadControlEventRecord = psGetListHead(psDRLCCustomDataStructure, eEventList, &plEventList);

    i=0;
    // move to specified index
    while((i<u8tableIndex) && (psLoadControlEventRecord != NULL))
    {
        psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetNext((DNODE *)psLoadControlEventRecord);
        i++;
    }

    if(psLoadControlEventRecord==NULL)
    {
        *ppsLoadControlEvent = NULL;
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return(E_SE_DRLC_EVENT_NOT_FOUND);
    }

    // fill in params
    *ppsLoadControlEvent = &psLoadControlEventRecord->sLoadControlEvent;

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCFindLoadControlEventRecord
 **
 ** DESCRIPTION:
 ** Finds Event record by specified u32IssuerId
 **
 ** PARAMETERS:                         Name                            Usage
 ** uint8                               u8SourceEndPointId              Source EP Id
 ** uint32                              u32IssuerId,                    Id to search for
 ** bool_t                              bIsServer,                      Search server or client list
 ** tsSE_DRLCLoadControlEventRecord     **ppsLoadControlEventRecord     Pointer to where to return the  pointer to the record found
 ** teSE_DRLCEventList                  *peEventList                    Pointer to where to return the event list the record was found in
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCFindLoadControlEventRecord(
            uint8                               u8SourceEndPointId,
            uint32                              u32IssuerId,
            bool_t                              bIsServer,
            tsSE_DRLCLoadControlEventRecord     **ppsLoadControlEventRecord,
            teSE_DRLCEventList                  *peEventList)
{

    uint8 u8DRLCClusterReturn;

    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    tsSE_DRLCCustomDataStructure   *psDRLCCustomDataStructure;

    if((ppsLoadControlEventRecord == NULL) || (peEventList == NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    // error check via EP number
    u8DRLCClusterReturn = eSE_DRLCFindDRLCCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psDRLCCustomDataStructure);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8DRLCClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    *ppsLoadControlEventRecord = psCheckToSeeIfEventExists(psDRLCCustomDataStructure, u32IssuerId, peEventList);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    if(*ppsLoadControlEventRecord==NULL)
    {
        return(E_SE_DRLC_EVENT_NOT_FOUND);
    }

    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCCancelLoadControlEvent
 **
 ** DESCRIPTION:
 ** Cancel LCE event
 **
 ** PARAMETERS:                 Name                            Usage
 ** uint8                       u8SourceEndPointId              Source EP Id
 ** uint8                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address               *psDestinationAddress           Destination Address
 ** tsCancelLoadControlEvent    *psLoadControlEvent             LoadControlEvent struct
 ** uint8                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCCancelLoadControlEvent(
                    uint8                           u8SourceEndPointId,
                    uint8                           u8DestinationEndPointId,
                    tsZCL_Address                   *psDestinationAddress,
                    tsSE_DRLCCancelLoadControlEvent *psCancelLoadControlEvent,
                    uint8                           *pu8TransactionSequenceNumber)
{

    uint8 u8DRLCClusterReturn;

    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;

    bool_t bIsZeroTime = FALSE;

    if (!bZCL_GetTimeHasBeenSynchronised())
    {
        return E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
    }

    // paramter checks
    if((psDestinationAddress== NULL) || (psCancelLoadControlEvent== NULL) || (pu8TransactionSequenceNumber== NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    // The time of zero will get replaced with the current time in eSE_DRLCCancelLoadControlEventNoMutex
    if (psCancelLoadControlEvent->u32effectiveTime == 0)
    {
        bIsZeroTime = TRUE;
    }

    // error check via EP number
    u8DRLCClusterReturn = eSE_DRLCFindDRLCCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psDRLCCustomDataStructure);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8DRLCClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    u8DRLCClusterReturn = eSE_DRLCCancelLoadControlEventNoMutex(psEndPointDefinition, psClusterInstance, psDRLCCustomDataStructure, psCancelLoadControlEvent);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // If the cancel is deferred, still send the cancel event immediately
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)

    	{
    	if( u8DRLCClusterReturn != E_SE_DRLC_CANCEL_DEFERRED)

    {
        return u8DRLCClusterReturn;
    }
    	}

    // send event automatically
    if (bIsZeroTime)
    {
        psCancelLoadControlEvent->u32effectiveTime = 0;
    }

    return eSE_DRLCCancelLoadControlEventSend(u8SourceEndPointId, u8DestinationEndPointId, psDestinationAddress, psCancelLoadControlEvent, pu8TransactionSequenceNumber);


}

/****************************************************************************
 **
 ** NAME:       eSE_DRLCCancelAllLoadControlEvents
 **
 ** DESCRIPTION:
 ** Cancel ALL LCE events
 **
** PARAMETERS:                          Name                            Usage
 ** uint8                               u8SourceEndPointId              Source EP Id
 ** uint8                               u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                       *psDestinationAddress           Destination Address
 ** teSE_DRLCCancelControl              eCancelEventControl             CanceControlEvent
 ** uint8                               *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCCancelAllLoadControlEvents(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    teSE_DRLCCancelControl      eCancelEventControl,
                    uint8                       *pu8TransactionSequenceNumber)
{

    uint8 u8DRLCClusterReturn;

    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;

    // paramter checks
    if((psDestinationAddress== NULL) || (pu8TransactionSequenceNumber== NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    // error check via EP number
    u8DRLCClusterReturn = eSE_DRLCFindDRLCCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psDRLCCustomDataStructure);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8DRLCClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // clear each list
    vRemoveAllEventsFromList(psDRLCCustomDataStructure, E_SE_DRLC_EVENT_LIST_SCHEDULED);
    vRemoveAllEventsFromList(psDRLCCustomDataStructure, E_SE_DRLC_EVENT_LIST_ACTIVE);
    vRemoveAllEventsFromList(psDRLCCustomDataStructure, E_SE_DRLC_EVENT_LIST_CANCELLED);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // send event automatically
    return eSE_DRLCCancelAllLoadControlEventsSend(u8SourceEndPointId, u8DestinationEndPointId, psDestinationAddress, eCancelEventControl, pu8TransactionSequenceNumber);

}

/****************************************************************************
 **
 ** NAME:       vRemoveAllEventsFromList
 **
 ** DESCRIPTION:
 ** Removes all table entries
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsSE_DRLCCustomDataStructure    psDRLCCustomDataStructure   Custom Data Structure
 ** teSE_DRLCEventList              eEventList                  Event List
 **
 ** RETURN:
 ** Nothing
 **
 ****************************************************************************/

PRIVATE  void vRemoveAllEventsFromList(
                        tsSE_DRLCCustomDataStructure    *psDRLCCustomDataStructure,
                        teSE_DRLCEventList              eEventList)
{
    DLIST *plEventList;
    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;

    psLoadControlEventRecord = psGetListHead(psDRLCCustomDataStructure, eEventList, &plEventList);

    if(psLoadControlEventRecord==NULL)
    {
        return;
    }

    // it appears that we don't reallu need to consider randomisation in the server - seeing as its going
    // to be different on each client device - we can't modle it hear on the scheduler
    // search through alloc list
    while(psLoadControlEventRecord!=NULL)
    {
        // remove from list
        psDLISTremove(plEventList, (DNODE *)psLoadControlEventRecord);
        // add to dealloc list
        vDLISTaddToTail(&psDRLCCustomDataStructure->lLoadControlEventDeAllocList, (DNODE *)(psLoadControlEventRecord));
        // get next
        psLoadControlEventRecord = (tsSE_DRLCLoadControlEventRecord *)psDLISTgetHead(plEventList);
    };

}

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

