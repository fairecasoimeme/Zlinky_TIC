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
 * COMPONENT:          PriceConversionFactorTableManager.c
 *
 * DESCRIPTION:        Maintain the list of Conversion Factor with their start time.
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

#ifdef PRICE_CONVERSION_FACTOR

PRIVATE bool_t boSearchForExisting( void *pvSearchParam, void *psNodeUnderTest);

PRIVATE void vCopyRecordIntoTable(
        tsSE_PriceConversionFactorRecord          *psTablePublishBlockPeriod,
        tsSE_PricePublishConversionCmdPayload     *psPublishConversionCmdPayload
       );

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/


/****************************************************************************
 **
 ** NAME:       eSE_PriceAddConversionFactorEntry
 **
 ** DESCRIPTION:
 ** Adds a Conversion Factor entry to the server.
 **
 ** PARAMETERS:                                 Name                                Usage
 **
 ** uint8                                       u8SourceEndPointId                  Source EP Id
 ** uint8                                       u8DestinationEndPointId             Dest   EP Id for the publish message
 ** tsZCL_Address                               *psDestinationAddress               Dest address for the publish message
 ** bool_t                                      bOverwritePrevious                  replace table if exists
 ** tsSE_PricePublishConversionCmdPayload       *psPublishConversionCmdPayload         Price information to add and send
 ** uint8                                       *pu8TransactionSequenceNumber       Returns the TSN of the message that was sent
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PriceAddConversionFactorEntry(
                    uint8                                   u8SourceEndPointId,
                    uint8                                   u8DestinationEndPointId,
                    tsZCL_Address                           *psDestinationAddress,
                    bool_t                                  bOverwritePrevious,
                    tsSE_PricePublishConversionCmdPayload          *psPublishConversionCmdPayload,
                    uint8                                   *pu8TransactionSequenceNumber)
{

    teSE_PriceStatus eAddPriceEntryReturn;
    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check the params
    if (psPublishConversionCmdPayload == NULL)
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


    eAddPriceEntryReturn = eSE_AddConversionFactorUsingPointer(u8SourceEndPointId, TRUE, psPriceCustomDataStructure,bOverwritePrevious, psPublishConversionCmdPayload);

    // send the message if it is OK
    if (eAddPriceEntryReturn == E_ZCL_SUCCESS)
    {
        // Get a new TSN because we are sending from API call rather than incoming message
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();

        eAddPriceEntryReturn = eSE_PricePublishConversionFactorSend(u8SourceEndPointId,
                                                         u8DestinationEndPointId,
                                                         psDestinationAddress,
                                                         pu8TransactionSequenceNumber,
                                                         psPublishConversionCmdPayload);
    }
    // release EP

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eAddPriceEntryReturn;
}

/****************************************************************************
 **
 ** NAME:       eSE_PriceGetConversionFactorEntry
 **
 ** DESCRIPTION:
 ** Gets a Price Conversion Factor entry based on table Index
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint8                       u8index                     Table Index
 ** tsSE_PricePublishConversionCmdPayload                    **ppsPublishConversionCmdPayload
 ** Written with a pointer to the price payload of the specified entry
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/
PUBLIC  teSE_PriceStatus eSE_PriceGetConversionFactorEntry(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint8                       u8tableIndex,
                    tsSE_PricePublishConversionCmdPayload    **ppsPublishConversionCmdPayload)
{
    uint8 u8Index;
    tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecord;
    teSE_PriceStatus eFindPriceClusterReturn;
    tsZCL_ClusterInstance *psClusterInstance;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    if (ppsPublishConversionCmdPayload == NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    *ppsPublishConversionCmdPayload = NULL;
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
    psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lConversionFactorAllocList);

    u8Index=0;

    // move to specified index
    while((u8Index<u8tableIndex) && (psPublishConversionFactorRecord != NULL))
    {
        psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetNext((DNODE *)psPublishConversionFactorRecord);
        u8Index++;
    }

    if(psPublishConversionFactorRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return(E_SE_PRICE_TABLE_NOT_FOUND);
    }

    // fill in params
    *ppsPublishConversionCmdPayload = &(psPublishConversionFactorRecord->sPublishConversionCmdPayload);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eSE_PriceRemoveConversionFactorEntry
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
PUBLIC  teSE_PriceStatus eSE_PriceRemoveConversionFactorEntry(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32StartTime)
{

    tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecord;
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
    psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lConversionFactorAllocList );
    psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTsearchForward((DNODE *)psPublishConversionFactorRecord, boSearchForExisting, (void *)&u32StartTime);

    if(psPublishConversionFactorRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_SE_PRICE_NOT_FOUND;
    }

    // remove from alloc list
    psDLISTremove(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psPublishConversionFactorRecord);
    // add to dealloc list
    vDLISTaddToTail(&psPriceCustomDataStructure->lConversionFactorDeAllocList, (DNODE *)(psPublishConversionFactorRecord));

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}


/****************************************************************************
 **
 ** NAME:       eSE_PriceDoesConversionFactorEntryExist
 **
 ** DESCRIPTION:
 ** Checks the table if COnversion Factor entry exists specified by the start time
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
PUBLIC  teSE_PriceStatus eSE_PriceDoesConversionFactorEntryExist(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32StartTime)
{
    tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecord;
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
    psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lConversionFactorAllocList);
    // search list for old entry
    psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTsearchForward((DNODE *)psPublishConversionFactorRecord, boSearchForExisting, (void *)&u32StartTime);

    if(psPublishConversionFactorRecord==NULL)
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
 ** NAME:       eSE_PriceClearAllConversionFactorEntries
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
PUBLIC  teSE_PriceStatus eSE_PriceClearAllConversionFactorEntries(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer)
{

    tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecord;
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
            psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lConversionFactorAllocList );
            if (psPublishConversionFactorRecord)
            {
                // remove from alloc list
                psDLISTremove(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psPublishConversionFactorRecord);
                // add to dealloc list
                vDLISTaddToTail(&psPriceCustomDataStructure->lConversionFactorDeAllocList, (DNODE *)(psPublishConversionFactorRecord));
            }
        } while(psPublishConversionFactorRecord!=NULL);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}

PUBLIC  teSE_PriceStatus eSE_AddConversionFactorUsingPointer(
            uint8                                   u8SourceEndPointId,
			bool_t                                    bIsServer,
            tsSE_PriceCustomDataStructure           *psPriceCustomDataStructure,
            bool_t                                  bOverwritePrevious,
            tsSE_PricePublishConversionCmdPayload  *psPublishConversionCmdPayload)
{

    tsSE_PriceConversionFactorRecord     *psPublishConversionFactorRecord, *psNextPublishConversionFactorRecord,
                                       *psPreviousPublishConversionFactorRecord, *psNewPublishConversionFactorRecord;
    teSE_PriceStatus eStatus;
    bool_t              bEqualNodeFound=FALSE;
    psPreviousPublishConversionFactorRecord = NULL;

    psNextPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lConversionFactorAllocList );


     while(  (psNextPublishConversionFactorRecord != NULL) &&
            (psNextPublishConversionFactorRecord->sPublishConversionCmdPayload.u32StartTime <= psPublishConversionCmdPayload->u32StartTime))
    {

        if( psNextPublishConversionFactorRecord->sPublishConversionCmdPayload.u32StartTime  == psPublishConversionCmdPayload->u32StartTime)
        {
            if( (psNextPublishConversionFactorRecord->sPublishConversionCmdPayload.u32IssuerEventId >= psPublishConversionCmdPayload->u32IssuerEventId ) && !bOverwritePrevious )
            {
                // We already have an event with event ID and start time so we reject
                return E_ZCL_FAIL;
            }
            else
            {
                 psDLISTremove(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *) psNextPublishConversionFactorRecord);
                 vDLISTaddToTail(&psPriceCustomDataStructure->lConversionFactorDeAllocList, (DNODE *)(psNextPublishConversionFactorRecord));
                 bEqualNodeFound = TRUE;
                 break;
            }

        }
        psPreviousPublishConversionFactorRecord = psNextPublishConversionFactorRecord;
        psNextPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord*)(psNextPublishConversionFactorRecord->dllConversionFactorNode.psNext);
    }


    psNewPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lConversionFactorDeAllocList);

    if(psNewPublishConversionFactorRecord==NULL)
    {

        psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetTail( &psPriceCustomDataStructure->lConversionFactorAllocList );

        if(psPublishConversionFactorRecord->sPublishConversionCmdPayload.u32StartTime <
                psPublishConversionCmdPayload->u32StartTime)
        {
            // We cannot add new Entry here, as this will cause problem.
            return E_ZCL_ERR_INSUFFICIENT_SPACE;
        }


       psDLISTremove(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psPublishConversionFactorRecord);
       vDLISTaddToHead(&psPriceCustomDataStructure->lConversionFactorDeAllocList, (DNODE *)psPublishConversionFactorRecord);

       eStatus = eSE_AddConversionFactorUsingPointer(u8SourceEndPointId, bIsServer, psPriceCustomDataStructure, bOverwritePrevious,
                                                            psPublishConversionCmdPayload);

        if(eStatus != E_ZCL_SUCCESS)
        {
            // restore last table back
            psDLISTremove(&psPriceCustomDataStructure->lConversionFactorDeAllocList, (DNODE *)psPublishConversionFactorRecord);
            vDLISTaddToTail(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psPublishConversionFactorRecord);
            return eStatus;
        }

        return E_ZCL_SUCCESS;
    }

    psDLISTremove(&psPriceCustomDataStructure->lConversionFactorDeAllocList, (DNODE *)psNewPublishConversionFactorRecord);
    if(bEqualNodeFound &&
       psPreviousPublishConversionFactorRecord != NULL &&
       psNewPublishConversionFactorRecord != NULL)
    {
         vDLISTinsertAfter(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psPreviousPublishConversionFactorRecord,
                         (DNODE *)psNewPublishConversionFactorRecord);

    }
    else if( bEqualNodeFound )
    {
        vDLISTaddToTail(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psNewPublishConversionFactorRecord);
    }
    else
    {
        if(psNextPublishConversionFactorRecord==NULL)
        {
            // add to tail
            vDLISTaddToTail(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psNewPublishConversionFactorRecord);
        }
        else
        {
            vDLISTinsertBefore(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psNextPublishConversionFactorRecord, (DNODE *)psNewPublishConversionFactorRecord);
        }
    }

    vCopyRecordIntoTable(psNewPublishConversionFactorRecord, psPublishConversionCmdPayload);

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
        &(((tsSE_PriceConversionFactorRecord *)psNodeUnderTest)->sPublishConversionCmdPayload.u32StartTime),
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

PRIVATE  void vCopyRecordIntoTable(tsSE_PriceConversionFactorRecord     *psTablePublishConversionFactor,
        tsSE_PricePublishConversionCmdPayload *psPublishConversionCmdPayload)

{

    // copy the whole structure
    memcpy(&psTablePublishConversionFactor->sPublishConversionCmdPayload, psPublishConversionCmdPayload,
            sizeof(tsSE_PricePublishConversionCmdPayload));


}

#endif /* BLOCK_CHARGING */
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

