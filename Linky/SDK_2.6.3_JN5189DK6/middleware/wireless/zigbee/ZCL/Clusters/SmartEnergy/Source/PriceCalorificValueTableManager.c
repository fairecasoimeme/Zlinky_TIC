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


/*****************************************************************************
 *
 * MODULE:             Price Cluster
 *
 * COMPONENT:          PriceCalorificValueTableManager.c
 *
 * DESCRIPTION:        Maintain the list of Calorific Value with their start time.
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
#include "Price.h"
#include "Price_internal.h"

#include "dbg.h"


#ifdef PRICE_CALORIFIC_VALUE

PRIVATE bool_t boSearchForExisting( void *pvSearchParam, void *psNodeUnderTest);
PRIVATE void vCopyRecordIntoTable(tsSE_PriceCalorificValueRecord  *psTablePublishBlockPeriod,tsSE_PricePublishCalorificValueCmdPayload      *psPublishCalorificValueCmdPayload);


/****************************************************************************
 **
 ** NAME:       eSE_PriceAddCalorificValueEntry
 **
 ** DESCRIPTION:
 ** Adds a Calorific Value Entry to the database and send out the publish calorific value message.  Only valid for a server.
 **
 ** PARAMETERS:                                 Name                                        Usage
 ** uint8                                       u8SourceEndPointId                      Source EP Id
 ** uint8                                       u8DestinationEndPointId                 Dest   EP Id for the publish message
 ** tsZCL_Address                               *psDestinationAddress                   Dest address for the publish message
 ** bool_t                                      bOverwritePrevious                      replace table if exists
 ** tsSE_PricePublishCalorificValueCmdPayload   *psPublishCalorificValueCmdPayload         Price information to add and send
 ** uint8                                       *pu8TransactionSequenceNumber              Returns the TSN of the message that was sent
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

 PUBLIC  teSE_PriceStatus eSE_PriceAddCalorificValueEntry(
                    uint8                                           u8SourceEndPointId,
                    uint8                                           u8DestinationEndPointId,
                    tsZCL_Address                                   *psDestinationAddress,
                    bool_t                                          bOverwritePrevious,
                    tsSE_PricePublishCalorificValueCmdPayload          *psPublishCalorificValueCmdPayload,
                    uint8                                           *pu8TransactionSequenceNumber)
{

    teSE_PriceStatus eAddPriceEntryReturn;
    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    if (psPublishCalorificValueCmdPayload == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

     if (!bZCL_GetTimeHasBeenSynchronised())
    {
         return E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
    }

     // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);

    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


     eAddPriceEntryReturn = eSE_AddCalorificValueEntryUsingPointer(u8SourceEndPointId, TRUE, psPriceCustomDataStructure,bOverwritePrevious, psPublishCalorificValueCmdPayload);
    // send the message if it is OK
     if (eAddPriceEntryReturn == E_ZCL_SUCCESS)
    {
        // Get a new TSN because we are sending from API call rather than incoming message
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        eAddPriceEntryReturn = eSE_PricePublishCalorificValueSend(u8SourceEndPointId,
                                                         u8DestinationEndPointId,
                                                         psDestinationAddress,
                                                         pu8TransactionSequenceNumber,
                                                         psPublishCalorificValueCmdPayload);
    }
    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return eAddPriceEntryReturn;
}

/****************************************************************************
 **
 ** NAME:       eSE_PriceGetCalorificValueEntry
 **
 ** DESCRIPTION:
 ** Gets a Price CalorificValue Factor entry based on table Index
 **
 ** PARAMETERS:                                     Name                        Usage
 **
 ** uint8                                           u8SourceEndPointId          Source EP Id
 ** bool_t                                         bIsServer                   Is server
 ** uint8                                           u8index                     Table Index
 ** tsSE_PricePublishCalorificValueCmdPayload                            **ppsPublishCalorificValueCmdPayload
 ** Written with a pointer to the price payload of the specified entry
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

//done

PUBLIC  teSE_PriceStatus eSE_PriceGetCalorificValueEntry(uint8  u8SourceEndPointId,bool_t  bIsServer,uint8  u8tableIndex,
                                                                                tsSE_PricePublishCalorificValueCmdPayload    **ppsPublishCalorificValueCmdPayload )

{
    uint8 u8Index;
    tsSE_PriceCalorificValueRecord *psPublishCalorificValueRecord;
    teSE_PriceStatus eFindPriceClusterReturn;
    tsZCL_ClusterInstance *psClusterInstance;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    if (ppsPublishCalorificValueCmdPayload == NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    *ppsPublishCalorificValueCmdPayload = NULL;
    // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);

    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    // get start of list
    psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lCalorificValueAllocList);
    u8Index=0;

    // move to specified index
    while((u8Index<u8tableIndex) && (psPublishCalorificValueRecord != NULL))
    {
        psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetNext((DNODE *)psPublishCalorificValueRecord);
        u8Index++;
    }

    if(psPublishCalorificValueRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

        return(E_SE_PRICE_TABLE_NOT_FOUND);
    }

    // fill in params
    *ppsPublishCalorificValueCmdPayload = &(psPublishCalorificValueRecord->sPublishCalorificValueCmdPayload);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eSE_PriceRemoveCalorificValueEntry
 **
 ** DESCRIPTION:
 ** Remove and entry, specified by the start time
 **
 ** PARAMETERS:                 Name                           Usage
 ** PARAMETERS:                 Name                        Usage
 **
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32StartTime                start time to search on
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

//done

PUBLIC  teSE_PriceStatus eSE_PriceRemoveCalorificValueEntry(
            uint8                       u8SourceEndPointId,
            bool_t                      bIsServer,
            uint32                      u32StartTime
)
{

    tsSE_PriceCalorificValueRecord *psPublishCalorificValueRecord;
    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // search through alloc list
    psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lCalorificValueAllocList );
    psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTsearchForward((DNODE *)psPublishCalorificValueRecord, boSearchForExisting, (void *)&u32StartTime);

    if(psPublishCalorificValueRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

        return E_SE_PRICE_NOT_FOUND;
    }

    // remove from alloc list
    psDLISTremove(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *)psPublishCalorificValueRecord);
    // add to dealloc list
    vDLISTaddToTail(&psPriceCustomDataStructure->lCalorificValueDeAllocList, (DNODE *)(psPublishCalorificValueRecord));

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}


/****************************************************************************
 **
 ** NAME:       eSE_PriceDoesCalorificValueEntryExist
 **
 ** DESCRIPTION:
 ** Checks the table if CalorificValue Factor entry exists specified by the start time
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32StartTime                start time to search on
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

//done

PUBLIC  teSE_PriceStatus eSE_PriceDoesCalorificValueEntryExist(uint8 u8SourceEndPointId, bool_t bIsServer, uint32 u32StartTime)
{
    tsSE_PriceCalorificValueRecord *psPublishCalorificValueRecord;
    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // search through alloc list
    psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lCalorificValueAllocList);
    // search list for old entry
    psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTsearchForward((DNODE *)psPublishCalorificValueRecord, boSearchForExisting, (void *)&u32StartTime);

    if(psPublishCalorificValueRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

        return E_SE_PRICE_NOT_FOUND;
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eSE_PriceClearAllCalorificValueEntries
 **
 ** DESCRIPTION:
 ** Removes all table entries
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

//done

PUBLIC  teSE_PriceStatus eSE_PriceClearAllCalorificValueEntries(uint8 u8SourceEndPointId, bool_t bIsServer)
{

    tsSE_PriceCalorificValueRecord *psPublishCalorificValueRecord;
    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // search through alloc list
    do {
            psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lCalorificValueAllocList );
            if (psPublishCalorificValueRecord)
            {
                // remove from alloc list
                psDLISTremove(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *)psPublishCalorificValueRecord);
                // add to dealloc list
                vDLISTaddToTail(&psPriceCustomDataStructure->lCalorificValueDeAllocList, (DNODE *)(psPublishCalorificValueRecord));
            }
        } while(psPublishCalorificValueRecord!=NULL);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eSE_AddCalorificValueEntryUsingPointer
 **
 ** DESCRIPTION:
 ** Adds CalorificValue Factor Entry to Table without using mutex
 **
 ** PARAMETERS:                             Name                                Usage
 ** uint8                                  u8SourceEndPointId                    Src End Point
 ** bool_t                                   bIsServer                            Server/Clent
 ** tsSE_PriceCustomDataStructure          *psPriceCustomDataStructure          list access
 ** bool_t                                  bOverwritePrevious                  overwrite y/n
 ** ttsSE_PricePublishCalorificValueCmdPayload *psPublishCalorificValueCmdPayload        table data
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_AddCalorificValueEntryUsingPointer(
            uint8                                   u8SourceEndPointId,
			bool_t                                    bIsServer,
            tsSE_PriceCustomDataStructure           *psPriceCustomDataStructure,
            bool_t                                  bOverwritePrevious,
            tsSE_PricePublishCalorificValueCmdPayload  *psPublishCalorificValueCmdPayload)
{

    tsSE_PriceCalorificValueRecord     *psPublishCalorificValueRecord, *psNextPublishCalorificValueRecord,
                                       *psPreviousPublishCalorificValueRecord, *psNewPublishCalorificValueRecord;
    teSE_PriceStatus eStatus;
    bool_t              bEqualNodeFound=FALSE;
    psPreviousPublishCalorificValueRecord = NULL;

    psNextPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lCalorificValueAllocList );

    // We iterate here to find Correct position to insert the passed PublishCalorificCmdPayload
    // based on u32StartTime.

     while(  (psNextPublishCalorificValueRecord != NULL) &&
            (psNextPublishCalorificValueRecord->sPublishCalorificValueCmdPayload.u32StartTime <= psPublishCalorificValueCmdPayload->u32StartTime))
    {

        if( psNextPublishCalorificValueRecord->sPublishCalorificValueCmdPayload.u32StartTime  == psPublishCalorificValueCmdPayload->u32StartTime)
        {
            if( (psNextPublishCalorificValueRecord->sPublishCalorificValueCmdPayload.u32IssuerEventId >= psPublishCalorificValueCmdPayload->u32IssuerEventId) && !bOverwritePrevious )
            {
                // We already have an event with event ID and start time so we reject
                return E_ZCL_FAIL;
            }
            else
            {
                 psDLISTremove(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *) psNextPublishCalorificValueRecord);
                 vDLISTaddToTail(&psPriceCustomDataStructure->lCalorificValueDeAllocList, (DNODE *)(psNextPublishCalorificValueRecord));
                 bEqualNodeFound = TRUE;
                 break;
            }

        }
        psPreviousPublishCalorificValueRecord = psNextPublishCalorificValueRecord;
        psNextPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord*)(psNextPublishCalorificValueRecord->dllCalorificValueNode.psNext);
    }


    psNewPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lCalorificValueDeAllocList);

    if(psNewPublishCalorificValueRecord==NULL)
    {

        psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetTail( &psPriceCustomDataStructure->lCalorificValueAllocList );

        if(psPublishCalorificValueRecord->sPublishCalorificValueCmdPayload.u32StartTime <
                psPublishCalorificValueCmdPayload->u32StartTime)
        {
            // We cannot add new Entry here, as this will cause problem.
            return E_ZCL_ERR_INSUFFICIENT_SPACE;
        }

        /* we can (possibly) fit it
         * remove tail from alloc and put it back on the free list
        */

        psDLISTremove(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *)psPublishCalorificValueRecord);
        vDLISTaddToHead(&psPriceCustomDataStructure->lCalorificValueDeAllocList, (DNODE *)psPublishCalorificValueRecord);

       eStatus = eSE_AddCalorificValueEntryUsingPointer(u8SourceEndPointId, bIsServer, psPriceCustomDataStructure, bOverwritePrevious,
                                                            psPublishCalorificValueCmdPayload);

        if(eStatus != E_ZCL_SUCCESS)
        {
            // restore last table back
            psDLISTremove(&psPriceCustomDataStructure->lCalorificValueDeAllocList, (DNODE *)psPublishCalorificValueRecord);
            vDLISTaddToTail(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *)psPublishCalorificValueRecord);
            return eStatus;
        }

        return E_ZCL_SUCCESS;
    }

    psDLISTremove(&psPriceCustomDataStructure->lCalorificValueDeAllocList, (DNODE *)psNewPublishCalorificValueRecord);
    if(bEqualNodeFound &&
       psPreviousPublishCalorificValueRecord != NULL &&
       psNewPublishCalorificValueRecord != NULL)
    {
         vDLISTinsertAfter(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *)psPreviousPublishCalorificValueRecord,
                         (DNODE *)psNewPublishCalorificValueRecord);

    }
    else if( bEqualNodeFound )
    {
         vDLISTaddToTail(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE*) psNewPublishCalorificValueRecord);
    }
    else
    {
        if(psNextPublishCalorificValueRecord==NULL)
        {
            // add to tail
            vDLISTaddToTail(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *)psNewPublishCalorificValueRecord);
        }
        else
        {
            vDLISTinsertBefore(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *)psNextPublishCalorificValueRecord, (DNODE *)psNewPublishCalorificValueRecord);
        }
    }

    vCopyRecordIntoTable(psNewPublishCalorificValueRecord, psPublishCalorificValueCmdPayload);
    // If we have just inserted a start time of now - update the Arrived fields
    if (psNewPublishCalorificValueRecord->sPublishCalorificValueCmdPayload.u32StartTime == 0)
    {
        psPriceCustomDataStructure->u32CalorificArrivalTimeOfStartNowEntry     = u32ZCL_GetUTCTime();
    }
    return(E_ZCL_SUCCESS);
}


/****************************************************************************
 **
 ** NAME:       boSearchForExisting
 **
 ** DESCRIPTION:
 ** Search helper function
 **
 ** PARAMETERS:             Name              Usage
 ** void                    *pvSearchParam    value to search on
 ** void                   *psNodeUnderTest  present node
 **
 ** RETURN:
 ** 0 - Success, -1 Fail
 **
 ****************************************************************************/
//done
PRIVATE  bool_t boSearchForExisting( void *pvSearchParam, void *psNodeUnderTest)
{
    uint32 u32ParamTime;
    uint32 u32NodeStartTime;

    // Memcpy to variables to avoid alignment exceptions
    memcpy(&u32NodeStartTime,
        &(((tsSE_PriceCalorificValueRecord *)psNodeUnderTest)->sPublishCalorificValueCmdPayload.u32StartTime),
        sizeof(uint32));

    memcpy(&u32ParamTime, pvSearchParam, sizeof(uint32));

    if (u32NodeStartTime == u32ParamTime)
    {
        return TRUE;
    }

    return FALSE;
}

/****************************************************************************
 **
 ** NAME:       vCopyRecordIntoTable
 **
 ** DESCRIPTION:
 ** Search helper function
 **
 ** PARAMETERS:               Name                              Usage
 ** tsSE_PricePublishPriceCmdPayload  *psTablePublishPriceCmdPayload    Database entry
 ** tsSE_PricePublishPriceCmdPayload  *psPublishPriceCmdPayload         temp entry to copy
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  void vCopyRecordIntoTable(tsSE_PriceCalorificValueRecord     *psTablePublishCalorificValue,
        tsSE_PricePublishCalorificValueCmdPayload *psPublishCalorificValueCmdPayload)

{

    // copy the whole structure
    memcpy(&psTablePublishCalorificValue->sPublishCalorificValueCmdPayload, psPublishCalorificValueCmdPayload,
            sizeof(tsSE_PricePublishCalorificValueCmdPayload));


}

#endif /* CALORIFIC VALUE */
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

