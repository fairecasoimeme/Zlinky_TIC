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
 * COMPONENT:          PriceBlockPeriodTableManager.c
 *
 * DESCRIPTION:        Maintain the list of Block Period Table events.
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



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

#ifdef BLOCK_CHARGING

PRIVATE bool_t boSearchForExisting( void *pvSearchParam, void *psNodeUnderTest);

PRIVATE teSE_PriceStatus eSE_CheckForTableOverlap(
                tsSE_PriceCustomDataStructure              *psPriceCustomDataStructure,
                tsSE_PricePublishBlockPeriodRecord      *psNextPublishBlockPeriodRecord,
                tsSE_PricePublishBlockPeriodRecord      *psPreviousPublishBlockPeriodRecord,
                uint32                                  u32StartTime,
                uint32                                  u32DurationInMinutes);

PRIVATE void vCopyRecordIntoTable(
        tsSE_PricePublishBlockPeriodRecord          *psTablePublishBlockPeriod,
        tsSE_PricePublishBlockPeriodCmdPayload    *psPublishBlockPeriodCmdPayload,
        tsSE_BlockThresholds                      *psBlockThresholds);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/


/****************************************************************************
 **
 ** NAME:       eSE_PriceAddBlockPeriodEntry
 **
 ** DESCRIPTION:
 ** Adds a block period table to the database and send out the publish block period message.  Only valid for a server.
 **
 ** PARAMETERS:                                 Name                                Usage
 ** uint8                                       u8SourceEndPointId                  Source EP Id
 ** uint8                                       u8DestinationEndPointId             Dest   EP Id for the publish message
 ** tsZCL_Address                               *psDestinationAddress               Dest address for the publish message
 ** bool_t                                      bOverwritePrevious                  replace table if exists
 ** tsSE_PricePublishBlockPeriodCmdPayload      *psPublishBlockPeriodCmdPayload     Price information to add and send
 ** tsSE_BlockThresholds                        *psBlockThresholds                    Block Thresholds
 ** uint8                                           *pu8TransactionSequenceNumber          Returns the TSN of the message that was sent
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/
PUBLIC  teSE_PriceStatus eSE_PriceAddBlockPeriodEntry(
                    uint8                                   u8SourceEndPointId,
                    uint8                                   u8DestinationEndPointId,
                    tsZCL_Address                           *psDestinationAddress,
                    bool_t                                  bOverwritePrevious,
                    tsSE_PricePublishBlockPeriodCmdPayload  *psPublishBlockPeriodCmdPayload,
                    tsSE_BlockThresholds                    *psBlockThresholds,
                    uint8                                   *pu8TransactionSequenceNumber)
{
    teSE_PriceStatus eAddPriceEntryReturn;

    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check the params
    if (psPublishBlockPeriodCmdPayload == NULL || psBlockThresholds == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    // Check no. of thresholds
    if (psBlockThresholds->u8NoOfBlockThreshold > CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    if (psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins == 0)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
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


    eAddPriceEntryReturn = eSE_AddBlockPeriodEntryUsingPointer(u8SourceEndPointId, TRUE, psPriceCustomDataStructure,
                            bOverwritePrevious, psPublishBlockPeriodCmdPayload, psBlockThresholds);

    // send the message if it is OK
    if (eAddPriceEntryReturn == E_ZCL_SUCCESS)
    {
        // Get a new TSN because we are sending from API call rather than incoming message
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        eAddPriceEntryReturn = eSE_PricePublishBlockPeriodSend(u8SourceEndPointId,
                                                         u8DestinationEndPointId,
                                                         psDestinationAddress,
                                                         pu8TransactionSequenceNumber,
                                                         psPublishBlockPeriodCmdPayload);
    }
    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return eAddPriceEntryReturn;
}

// Unit tests need a way to add block periods to a client list.  This is similar to above function
PUBLIC  teSE_PriceStatus eSE_PriceAddBlockPeriodEntryToClient(
                    uint8                                       u8SourceEndPointId,
                    bool_t                                      bOverwritePrevious,
                    tsSE_PricePublishBlockPeriodCmdPayload        *psBlockPeriodPayload)
{
    teSE_PriceStatus eAddPriceEntryReturn;

    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check the params
    if (psBlockPeriodPayload == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    if (psBlockPeriodPayload->u32BlockPeriodDurationInMins == 0)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    if (!bZCL_GetTimeHasBeenSynchronised())
    {
         return E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
    }

    // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, FALSE, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    eAddPriceEntryReturn = eSE_AddBlockPeriodEntryUsingPointer(u8SourceEndPointId, FALSE, psPriceCustomDataStructure, bOverwritePrevious, psBlockPeriodPayload, NULL);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eAddPriceEntryReturn;

}

/****************************************************************************
 **
 ** NAME:       eSE_PriceGetBlockPeriodEntry
 **
 ** DESCRIPTION:
 ** Gats a price table specified by index
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint8                       u8index                     Table Index
 ** tsSE_PricePublishBlockPeriodCmdPayload    **ppsPublishBlockPeriodCmdPayload - Written with a pointer to the price payload of the specified entry
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PriceGetBlockPeriodEntry(
            uint8                       u8SourceEndPointId,
            bool_t                      bIsServer,
            uint8                       u8tableIndex,
            tsSE_PricePublishBlockPeriodCmdPayload    **ppsPublishBlockPeriodCmdPayload )

{
    uint8 u8Index;

    tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord;

    teSE_PriceStatus eFindPriceClusterReturn;
    tsZCL_ClusterInstance *psClusterInstance;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    if (ppsPublishBlockPeriodCmdPayload == NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }
    *ppsPublishBlockPeriodCmdPayload = NULL;

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
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lBlockPeriodAllocList);

    u8Index=0;
    // move to specified index
    while((u8Index<u8tableIndex) && (psPublishBlockPeriodRecord != NULL))
    {
        psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetNext((DNODE *)psPublishBlockPeriodRecord);
        u8Index++;
    }

    if(psPublishBlockPeriodRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return(E_SE_PRICE_TABLE_NOT_FOUND);
    }

    // fill in params
    *ppsPublishBlockPeriodCmdPayload = &(psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eSE_PriceRemoveBlockPeriodEntry
 **
 ** DESCRIPTION:
 ** Remove and entry, specified by the start time
 **
 ** PARAMETERS:         Name                    Usage
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32StartTime                start time to search on
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PriceRemoveBlockPeriodEntry(
            uint8                       u8SourceEndPointId,
            bool_t                      bIsServer,
            uint32                      u32StartTime
)
{
    tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord;

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
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lBlockPeriodAllocList );
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTsearchForward((DNODE *)psPublishBlockPeriodRecord, boSearchForExisting, (void *)&u32StartTime);

    if(psPublishBlockPeriodRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_SE_PRICE_NOT_FOUND;
    }

    // remove from alloc list
    psDLISTremove(&psPriceCustomDataStructure->lBlockPeriodAllocList, (DNODE *)psPublishBlockPeriodRecord);
    // add to dealloc list
    vDLISTaddToTail(&psPriceCustomDataStructure->lBlockPeriodDeAllocList, (DNODE *)(psPublishBlockPeriodRecord));

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eSE_PriceDoesBlockPeriodEntryExist
 **
 ** DESCRIPTION:
 **Checks for a table entry exixting, specified by the start time
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

PUBLIC  teSE_PriceStatus eSE_PriceDoesBlockPeriodEntryExist(uint8 u8SourceEndPointId, bool_t bIsServer, uint32 u32StartTime)
{
    tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord;

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
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lBlockPeriodAllocList);
    // search list for old entry
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTsearchForward((DNODE *)psPublishBlockPeriodRecord, boSearchForExisting, (void *)&u32StartTime);

    if(psPublishBlockPeriodRecord==NULL)
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
 ** NAME:       eSE_PriceClearAllBlockPeriodEntries
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

PUBLIC  teSE_PriceStatus eSE_PriceClearAllBlockPeriodEntries(uint8 u8SourceEndPointId, bool_t bIsServer)
{
    tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord;

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
        psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lBlockPeriodAllocList );
        if (psPublishBlockPeriodRecord)
        {
            // remove from alloc list
            psDLISTremove(&psPriceCustomDataStructure->lBlockPeriodAllocList, (DNODE *)psPublishBlockPeriodRecord);
            // add to dealloc list
            vDLISTaddToTail(&psPriceCustomDataStructure->lBlockPeriodDeAllocList, (DNODE *)(psPublishBlockPeriodRecord));
        }
    } while(psPublishBlockPeriodRecord!=NULL);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eSE_AddBlockPeriodEntryUsingPointer
 **
 ** DESCRIPTION:
 ** Block Period table add function used by even handler - no mutex in this
 **
 ** PARAMETERS:                             Name                                Usage
 ** uint8                                     u8SourceEndPointId                    Src End Point
 ** bool_t                                    bIsServer                            Server/Clent
 ** tsSE_PriceCustomDataStructure             *psPriceCustomDataStructure          list access
 ** bool_t                                  bOverwritePrevious                  overwrite y/n
 ** ttsSE_PricePublishBlockPeriodCmdPayload *psPublishBlockPeriodCmdPayload        table data
 ** tsSE_BlockThresholds                    *psBlockThresholds                    Block Thresholds
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_AddBlockPeriodEntryUsingPointer(
            uint8                                     u8SourceEndPointId,
			bool_t                                    bIsServer,
            tsSE_PriceCustomDataStructure             *psPriceCustomDataStructure,
            bool_t                                  bOverwritePrevious,
            tsSE_PricePublishBlockPeriodCmdPayload  *psPublishBlockPeriodCmdPayload,
            tsSE_BlockThresholds                    *psBlockThresholds)
{
    tsSE_PricePublishBlockPeriodRecord     *psPublishBlockPeriodRecord, *psNextPublishBlockPeriodRecord,
                                        *psPreviousPublishBlockPeriodRecord, *psNewPublishBlockPeriodRecord;
    teSE_PriceStatus eStatus;
    bool_t bAddThresholds = FALSE;

    ////////////////////////////////////////////////////////////////////////
    // Initial checks and setup
    ////////////////////////////////////////////////////////////////////////

    // Check entry for 32 bit time overflow
    if (psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime +
            60*psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins < psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime)
    {
        if (psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins != E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED)
           return E_SE_BLOCK_PERIOD_OVERFLOW;
        // Duration of until changed is OK up to 60 seconds of end of time
        if (psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime > 0xffffffff-60)
            return E_SE_BLOCK_PERIOD_OVERFLOW;
    }


    // check to see if the entry exists already - search through alloc list based on start time
    // Search the list maintaining prev and next pointers.  Stop when next pointer is later than current time and
    // prev is older or equal to current time.
    psPreviousPublishBlockPeriodRecord = NULL;
    psNextPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lBlockPeriodAllocList );

    /* check alloc head is empty or not */
    if(!psNextPublishBlockPeriodRecord)
    {
        bAddThresholds = TRUE;  /* Add thresholds to the Attr Record */
    }
    // No need to make adjustment for start time of zero here as we want 0 to always be first in the search
    while(  (psNextPublishBlockPeriodRecord != NULL) &&
            (psNextPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime <= psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime))
    {
        psPreviousPublishBlockPeriodRecord = psNextPublishBlockPeriodRecord;
        psNextPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord*)(psNextPublishBlockPeriodRecord->dllBlockPeriodNode.psNext);
    }

    // Does new event overlap
    eStatus = eSE_CheckForTableOverlap(psPriceCustomDataStructure, psNextPublishBlockPeriodRecord,
            psPreviousPublishBlockPeriodRecord, psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime,
            psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins);


    if(eStatus!= E_ZCL_SUCCESS)
    {
        // bOverwritePrevious may not be set on a server that is using the API to add to the list
        if (!bOverwritePrevious)
            return eStatus;
        // If its a "duplicate" return now and silently discard it.  Duplicate must have same issuer ID.
        if (psPreviousPublishBlockPeriodRecord != NULL &&
            psPreviousPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32IssuerEventId == psPublishBlockPeriodCmdPayload->u32IssuerEventId &&
            psPreviousPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime == psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime &&
            (psPreviousPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins == psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins ||
                    E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED== psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins))
        {
            return E_SE_BLOCK_PERIOD_DUPLICATE;
        }


        // Incoming event with start of now - need to compare with first entry in the list.
        // This could be next or prev depending on what the first entry in the list was.
        // duration may be adjusted down
        if (psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime == 0)
        {
            tsSE_PricePublishBlockPeriodRecord *psHeadRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lBlockPeriodAllocList );
            if (psHeadRecord != NULL &&
                psHeadRecord->sPublishBlockPeriodCmdPayload.u32IssuerEventId == psPublishBlockPeriodCmdPayload->u32IssuerEventId &&
                (psHeadRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins >= psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins ||
                        E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED == psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins))

            {
                return E_SE_BLOCK_PERIOD_DUPLICATE;
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////
    // Delete all overlapping events
    ////////////////////////////////////////////////////////////////////////
    // Overlap allowed - Newer issuer ID always removes ALL overlaps with older ID.
    // Remove any overlap with previous event
    eStatus = eSE_CheckForTableOverlap(psPriceCustomDataStructure, NULL,
            psPreviousPublishBlockPeriodRecord, psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime,
            psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins);
    if(eStatus != E_ZCL_SUCCESS)
    {
        uint32 u32CurrentTime = u32ZCL_GetUTCTime();
        // Check new-ness.  If the event ID of the new item is not newer, this is an error and no delete will be done.
        if(psPreviousPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32IssuerEventId >=
                psPublishBlockPeriodCmdPayload->u32IssuerEventId)
            return E_SE_BLOCK_PERIOD_DATA_OLD;

        // Special rule if the event to be deleted is currently running and the incoming overlapping event is
        // in the future.  The current event shouldn't be deleted  but its time adjusted to end when the new event begins.
        if (u32CurrentTime >= psPreviousPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime &&
            u32CurrentTime < psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime)
        {
            psPreviousPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins = E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED;
        }
        else
        {
            // Delete the previous record
            psDLISTremove(&psPriceCustomDataStructure->lBlockPeriodAllocList, (DNODE *)psPreviousPublishBlockPeriodRecord);
            vDLISTaddToHead(&psPriceCustomDataStructure->lBlockPeriodDeAllocList, (DNODE *)psPreviousPublishBlockPeriodRecord);
        }
    }



    // Now remove any overlapping following nodes - may be more than one.  psNextPublishBlockPeriodRecord is kept up to date
    do
    {
        DNODE *pTempNode;
        // Check for overlap returns success if both next and prev are NULL.
        eStatus = eSE_CheckForTableOverlap(psPriceCustomDataStructure, psNextPublishBlockPeriodRecord, NULL,
                psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime,
                psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins);
        if(eStatus != E_ZCL_SUCCESS)
        {
            // We fail if we find any existing events that have a newer or equall issuer ID
            // This could leave us having already deleted some older overlapping events and then
            // not adding the new one.  We don't know the server's intention in this case,
            // so doing this is as valid as checking and aborting with the list intact.
            if(psNextPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32IssuerEventId >=
                    psPublishBlockPeriodCmdPayload->u32IssuerEventId)
                return E_SE_BLOCK_PERIOD_DATA_OLD;

            // Delete and move next pointer on
            pTempNode = (DNODE *)(psNextPublishBlockPeriodRecord);
            psNextPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord*)(pTempNode->psNext);
            psDLISTremove(&psPriceCustomDataStructure->lBlockPeriodAllocList, pTempNode);
            vDLISTaddToHead(&psPriceCustomDataStructure->lBlockPeriodDeAllocList, pTempNode);
        }
    }
    while(eStatus != E_ZCL_SUCCESS);


    ////////////////////////////////////////////////////////////////////////
    // Allocate a buffer for the new item
    ////////////////////////////////////////////////////////////////////////

    // we need to get a list item from the dealloc list
    // If the dealloc list is empty, we may be able to delete the last item on the list then make a recursive call to insert the item.
    psNewPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lBlockPeriodDeAllocList);
    if(psNewPublishBlockPeriodRecord==NULL)
    {
        // get tail of alloc list
        psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetTail( &psPriceCustomDataStructure->lBlockPeriodAllocList );
        // see if we can fit the new table in if has a start time earlier than the last entry in the list
        // Time takes priority over issuer ID on a full list.
        if(psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime <
                psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime)
        {
            return E_ZCL_ERR_INSUFFICIENT_SPACE;
        }
        // we can (possibly) fit it
        // remove tail from alloc and put it back on the free list
        psDLISTremove(&psPriceCustomDataStructure->lBlockPeriodAllocList, (DNODE *)psPublishBlockPeriodRecord);
        vDLISTaddToHead(&psPriceCustomDataStructure->lBlockPeriodDeAllocList, (DNODE *)psPublishBlockPeriodRecord);
        // try to add again
        eStatus = eSE_AddBlockPeriodEntryUsingPointer(u8SourceEndPointId, bIsServer, psPriceCustomDataStructure, bOverwritePrevious,
                psPublishBlockPeriodCmdPayload, psBlockThresholds);
        if(eStatus != E_ZCL_SUCCESS)
        {
            // restore last table back
            psDLISTremove(&psPriceCustomDataStructure->lBlockPeriodDeAllocList, (DNODE *)psPublishBlockPeriodRecord);
            vDLISTaddToTail(&psPriceCustomDataStructure->lBlockPeriodAllocList, (DNODE *)psPublishBlockPeriodRecord);
            return eStatus;
        }

        return E_ZCL_SUCCESS;
    }

    ////////////////////////////////////////////////////////////////////////
    // Add the new buffer to the list
    ////////////////////////////////////////////////////////////////////////

    // When we get to here, the list has been cleaned of overlap and a free item has been allocated for us.
    // What could possibly go wrong.....

    // remove from dealloc list
    psDLISTremove(&psPriceCustomDataStructure->lBlockPeriodDeAllocList, (DNODE *)psNewPublishBlockPeriodRecord);

    // add to alloc list
    if(psNextPublishBlockPeriodRecord==NULL)
    {
        // add to tail
        vDLISTaddToTail(&psPriceCustomDataStructure->lBlockPeriodAllocList, (DNODE *)psNewPublishBlockPeriodRecord);
    }
    else
    {
        vDLISTinsertBefore(&psPriceCustomDataStructure->lBlockPeriodAllocList, (DNODE *)psNextPublishBlockPeriodRecord, (DNODE *)psNewPublishBlockPeriodRecord);
    }

       vCopyRecordIntoTable(psNewPublishBlockPeriodRecord, psPublishBlockPeriodCmdPayload,
                psBlockThresholds);

    // If we have just inserted a start time of now - update the Arrived fields
    if (psNewPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime == 0)
    {
        psPriceCustomDataStructure->u32BlockPeriodArrivalTimeOfStartNowEntry     = u32ZCL_GetUTCTime();
        psPriceCustomDataStructure->u32BlockPeriodArrivalDurationOfStartNowEntry = psNewPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins;

        bAddThresholds = TRUE;
    }

    /* Record is added at the head of list, add thresholds to Attr Record */
    if(bIsServer && bAddThresholds)
    {
        eSE_PriceAddBlockThresholds(u8SourceEndPointId, psBlockThresholds);
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eSE_CheckForTableOverlap
 **
 ** DESCRIPTION:
 ** checks to see if table times overlap
 **
 ** PARAMETERS:                             Name                                    Usage
 ** tsSE_PriceCustomDataStructure             *psPriceCustomDataStructure,              original start and duration of list event with start now
 ** tsSE_PricePublishBlockPeriodRecord      *psNextPublishBlockPeriodRecord         table data
 ** tsSE_PricePublishBlockPeriodRecord      *psPreviousPublishBlockPeriodRecord     table data
 ** uint32                                     u32StartTime                             start time
 ** uint32                                     u32DurationInMinutes                     duration
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PRIVATE  teSE_PriceStatus eSE_CheckForTableOverlap(
                tsSE_PriceCustomDataStructure              *psPriceCustomDataStructure,
                tsSE_PricePublishBlockPeriodRecord      *psNextPublishBlockPeriodRecord,
                tsSE_PricePublishBlockPeriodRecord      *psPreviousPublishBlockPeriodRecord,
                uint32                                  u32StartTime,
                uint32                                  u32DurationInMinutes)
{
    // check next - if not at end of list
    if (psNextPublishBlockPeriodRecord !=NULL)
    {
        uint32 u32AdjustedStartTime =  (u32StartTime == 0) ? u32ZCL_GetUTCTime() : u32StartTime;
        if (u32DurationInMinutes == E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED)
        {
            // Don't allow two events at same time but even one second is allowed
            // This could be a sneaky way to get events of duration less than a minute.
            if (u32AdjustedStartTime + 1 > psNextPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime)
            {
                return(E_SE_BLOCK_PERIOD_OVERLAP);
            }
        }
        else
        {
            // Only adjust the incoming - if the next is the start of the list must be comparing zero with zero so will overlap
            if (u32AdjustedStartTime + u32DurationInMinutes*60 > psNextPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime )
            {
                return(E_SE_BLOCK_PERIOD_OVERLAP);
            }
        }
    }

    // check previous - if not at start of list
    if (psPreviousPublishBlockPeriodRecord !=NULL)
    {
        // Make allowance for the first item in the list having the special start time of zero
        // Leave the incoming item at its normal time - if its zero it will always overlap an earlier item
        uint32 u32AdjustedPreviousStartTime = psPreviousPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime;
        uint32 u32AdjustedPreviousDuration  = psPreviousPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins;
        if (u32AdjustedPreviousStartTime == 0)
        {
            // Check the start time against the original arrival parameters.  If we try to use the current duration
            // we are likely to run into rounding problems with the duration in minutes and the times in seconds.
            u32AdjustedPreviousStartTime = psPriceCustomDataStructure->u32BlockPeriodArrivalTimeOfStartNowEntry;
            u32AdjustedPreviousDuration  = psPriceCustomDataStructure->u32BlockPeriodArrivalDurationOfStartNowEntry;
        }
        if (u32AdjustedPreviousDuration == E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED)
        {
            if (u32AdjustedPreviousStartTime + 1 > u32StartTime)
            {
                return(E_SE_BLOCK_PERIOD_OVERLAP);
            }
        }
        else
        {
            if (u32AdjustedPreviousStartTime + u32AdjustedPreviousDuration*60 > u32StartTime)
            {
                return(E_SE_BLOCK_PERIOD_OVERLAP);
            }
        }
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
 ** PARAMETERS:             				Name              Usage
 ** void                    				*pvSearchParam    value to search on
 ** void                   *psNodeUnderTest  present node
 **
 ** RETURN:
 ** 0 - Success, -1 Fail
 **
 ****************************************************************************/

PRIVATE  bool_t boSearchForExisting( void *pvSearchParam, void *psNodeUnderTest)
{
    uint32 u32ParamTime;
    uint32 u32NodeStartTime;

    // Memcpy to variables to avoid alignment exceptions
    memcpy(&u32NodeStartTime,
        &(((tsSE_PricePublishBlockPeriodRecord *)psNodeUnderTest)->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime),
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

PRIVATE  void vCopyRecordIntoTable(tsSE_PricePublishBlockPeriodRecord     *psTablePublishBlockPeriod,
        tsSE_PricePublishBlockPeriodCmdPayload *psPublishBlockPeriodCmdPayload,
        tsSE_BlockThresholds                    *psBlockThresholds)
{

    // copy the whole structure
    memcpy(&psTablePublishBlockPeriod->sPublishBlockPeriodCmdPayload, psPublishBlockPeriodCmdPayload,
            sizeof(tsSE_PricePublishBlockPeriodCmdPayload));

    /* check psBlockThresholds pointer: Null means cluster else server */
    if(psBlockThresholds)
    {
        memcpy(&psTablePublishBlockPeriod->sBlockThresholds, psBlockThresholds,
                sizeof(tsSE_BlockThresholds));
    }


}

#endif /* BLOCK_CHARGING */
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

