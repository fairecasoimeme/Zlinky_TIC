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
 * COMPONENT:          PriceTableManager.c
 *
 * DESCRIPTION:        Maintain the list of price events.
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
typedef struct {
    tsSE_PriceCustomDataStructure  *psPriceCustomDataStructure;
    uint32                      u32Time;
} tsExpiredCheckSeachStruct;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
//PRIVATE bool_t boSearchForInsertPoint( void *pvSearchParam, DNODE *psNodeUnderTest);
PRIVATE bool_t boSearchForExisting( void *pvSearchParam, void *psNodeUnderTest);

PRIVATE teSE_PriceStatus eSE_CheckForTableOverlap(
                tsSE_PriceCustomDataStructure      *psPriceCustomDataStructure,
                tsSE_PricePublishPriceRecord       *psNextPublishPriceRecord,
                tsSE_PricePublishPriceRecord       *psPreviousPublishPriceRecord,
                uint32                      u32StartTime,
                uint16                      u16DurationInMinutes);

PRIVATE void vCopyRecordIntoTable(
                tsSE_PricePublishPriceCmdPayload    *psTablePublishPriceCmdPayload,
                tsSE_PricePublishPriceCmdPayload    *psPublishPriceCmdPayload);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eSE_PriceAddPriceEntry
 **
 ** DESCRIPTION:
 ** Adds a price table to the database and send out the publish price message.  Only valid for a server.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** uint8                       u8DestinationEndPointId     Dest   EP Id for the publish message
 ** tsZCL_Address               *psDestinationAddress       Dest address for the publish message
 ** bool_t                      bOverwritePrevious          replace table if exists
 ** tsSE_PricePublishPriceCmdPayload    *psPricePayload     Price information to add and send
 ** uint8                       *pu8TransactionSequenceNumber  Returns the TSN of the message that was sent
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/
PUBLIC  teSE_PriceStatus eSE_PriceAddPriceEntry(
                    uint8                               u8SourceEndPointId,
                    uint8                               u8DestinationEndPointId,
                    tsZCL_Address                       *psDestinationAddress,
                    bool_t                              bOverwritePrevious,
                    tsSE_PricePublishPriceCmdPayload    *psPricePayload,
                    uint8                               *pu8TransactionSequenceNumber)
{
    uint8 u8AddPriceEntryReturn;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check the params
    if (psPricePayload == NULL || psPricePayload->sRateLabel.pu8Data == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    // Check if the rate label is too long
    if (psPricePayload->sRateLabel.u8Length > SE_PRICE_SERVER_MAX_STRING_LENGTH)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    if (psPricePayload->u16DurationInMinutes == 0)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    if (!bZCL_GetTimeHasBeenSynchronised())
    {
         return E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
    }

    // error check via EP number
    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    u8AddPriceEntryReturn = eSE_AddPriceEntryUsingPointer(psPriceCustomDataStructure, bOverwritePrevious, psPricePayload);

    // send the message if it is OK
    if (u8AddPriceEntryReturn == E_ZCL_SUCCESS)
    {
        // Get a new TSN because we are sending from API call rather than incoming message
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        u8AddPriceEntryReturn = eSE_PricePublishPriceSend(u8SourceEndPointId,
                                                         u8DestinationEndPointId,
                                                         psDestinationAddress,
                                                         pu8TransactionSequenceNumber,
                                                         psPricePayload);
    }
    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return u8AddPriceEntryReturn;

}

// Unit tests need a way to add prices to a client list.  This is similar to above function
PUBLIC  teSE_PriceStatus eSE_PriceAddPriceEntryToClient(
                    uint8                               u8SourceEndPointId,
                    bool_t                              bOverwritePrevious,
                    tsSE_PricePublishPriceCmdPayload    *psPricePayload)
{
    uint8 u8AddPriceEntryReturn;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check the params
    if (psPricePayload == NULL || psPricePayload->sRateLabel.pu8Data == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    if (psPricePayload->u16DurationInMinutes == 0)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    if (!bZCL_GetTimeHasBeenSynchronised())
    {
         return E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
    }

    // error check via EP number
    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, FALSE, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    u8AddPriceEntryReturn = eSE_AddPriceEntryUsingPointer(psPriceCustomDataStructure, bOverwritePrevious, psPricePayload);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return u8AddPriceEntryReturn;

}

/****************************************************************************
 **
 ** NAME:       eSE_PriceGetPriceEntry
 **
 ** DESCRIPTION:
 ** Gats a price table specified by index
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint8                       u8index                     Table Index
 ** tsSE_PricePublishPriceCmdPayload    **ppsPricePayload - Written with a pointer to the price payload of the specified entry
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PriceGetPriceEntry(
            uint8                       u8SourceEndPointId,
            bool_t                      bIsServer,
            uint8                       u8tableIndex,
            tsSE_PricePublishPriceCmdPayload    **ppsPricePayload)

{
    int i;

    tsSE_PricePublishPriceRecord *psPublishPriceRecord;

    uint8 u8FindPriceClusterReturn;
    tsZCL_ClusterInstance *psClusterInstance;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    if (ppsPricePayload == NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }
    *ppsPricePayload = NULL;

    // error check via EP number
    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // get start of list
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lPriceAllocList);

    i=0;
    // move to specified index
    while((i<u8tableIndex) && (psPublishPriceRecord != NULL))
    {
        psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetNext((DNODE *)psPublishPriceRecord);
        i++;
    }

    if(psPublishPriceRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return(E_SE_PRICE_TABLE_NOT_FOUND);
    }

    // fill in params
    *ppsPricePayload = &(psPublishPriceRecord->sPublishPriceCmdPayload);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eSE_PriceRemovePriceEntry
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

PUBLIC  teSE_PriceStatus eSE_PriceRemovePriceEntry(
            uint8                       u8SourceEndPointId,
            bool_t                      bIsServer,
            uint32                      u32StartTime
)
{
    tsSE_PricePublishPriceRecord *psPublishPriceRecord;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check via EP number
    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // search through alloc list
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lPriceAllocList );
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTsearchForward((DNODE *)psPublishPriceRecord, boSearchForExisting, (void *)&u32StartTime);

    if(psPublishPriceRecord==NULL)
    {
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_SE_PRICE_NOT_FOUND;
    }

    // remove from alloc list
    psDLISTremove(&psPriceCustomDataStructure->lPriceAllocList, (DNODE *)psPublishPriceRecord);
    // add to dealloc list
    vDLISTaddToTail(&psPriceCustomDataStructure->lPriceDeAllocList, (DNODE *)(psPublishPriceRecord));

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eSE_PriceDoesPriceEntryExist
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

PUBLIC  teSE_PriceStatus eSE_PriceDoesPriceEntryExist(uint8 u8SourceEndPointId, bool_t bIsServer, uint32 u32StartTime)
{
    tsSE_PricePublishPriceRecord *psPublishPriceRecord;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check via EP number
    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // search through alloc list
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lPriceAllocList);
    // search list for old entry
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTsearchForward((DNODE *)psPublishPriceRecord, boSearchForExisting, (void *)&u32StartTime);

    if(psPublishPriceRecord==NULL)
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
 ** NAME:       eSE_PriceClearAllPriceEntries
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

PUBLIC  teSE_PriceStatus eSE_PriceClearAllPriceEntries(uint8 u8SourceEndPointId, bool_t bIsServer)
{
    tsSE_PricePublishPriceRecord *psPublishPriceRecord;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    // error check via EP number
    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // search through alloc list
    do {
        psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lPriceAllocList );
        if (psPublishPriceRecord)
        {
            // remove from alloc list
            psDLISTremove(&psPriceCustomDataStructure->lPriceAllocList, (DNODE *)psPublishPriceRecord);
            // add to dealloc list
            vDLISTaddToTail(&psPriceCustomDataStructure->lPriceDeAllocList, (DNODE *)(psPublishPriceRecord));
        }
    } while(psPublishPriceRecord!=NULL);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eSE_AddPriceEntryUsingPointer
 **
 ** DESCRIPTION:
 ** price table add function used by even handler - no mutex in this
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsSE_PriceCustomDataStructure *psPriceCustomDataStructure  list access
 ** bool_t                      bOverwritePrevious          overwrite y/n
 ** tsSE_PricePublishPriceCmdPayload   *psPublishPriceCmdPayload    table data
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_AddPriceEntryUsingPointer(
            tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
            bool_t                      bOverwritePrevious,
            tsSE_PricePublishPriceCmdPayload   *psPublishPriceCmdPayload)
{
    tsSE_PricePublishPriceRecord *psPublishPriceRecord, *psNextPublishPriceRecord, *psPreviousPublishPriceRecord, *psNewPublishPriceRecord;
    uint8 u8Status;

    ////////////////////////////////////////////////////////////////////////
    // Initial checks and setup
    ////////////////////////////////////////////////////////////////////////

    // Check entry for 32 bit time overflow
    if (psPublishPriceCmdPayload->u32StartTime + 60*psPublishPriceCmdPayload->u16DurationInMinutes < psPublishPriceCmdPayload->u32StartTime)
    {
        if (psPublishPriceCmdPayload->u16DurationInMinutes != E_SE_PRICE_DURATION_UNTIL_CHANGED)
           return E_SE_PRICE_OVERFLOW;
        // Duration of until changed is OK up to 60 seconds of end of time
        if (psPublishPriceCmdPayload->u32StartTime > 0xffffffff-60)
            return E_SE_PRICE_OVERFLOW;
    }


    // check to see if the entry exists already - search through alloc list based on start time
    // Search the list maintaining prev and next pointers.  Stop when next pointer is later than current time and
    // prev is older or equall to current time.
    psPreviousPublishPriceRecord = NULL;
    psNextPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lPriceAllocList );

    // No need to make adjustment for start time of zero here as we want 0 to always be first in the search
    while((psNextPublishPriceRecord != NULL) && (psNextPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime <= psPublishPriceCmdPayload->u32StartTime))
    {
        psPreviousPublishPriceRecord = psNextPublishPriceRecord;
        psNextPublishPriceRecord = (tsSE_PricePublishPriceRecord*)(psNextPublishPriceRecord->dllPriceNode.psNext);
    }

    // Does new event overlap
    u8Status = eSE_CheckForTableOverlap(psPriceCustomDataStructure, psNextPublishPriceRecord, psPreviousPublishPriceRecord, psPublishPriceCmdPayload->u32StartTime, psPublishPriceCmdPayload->u16DurationInMinutes);
    if(u8Status!= E_ZCL_SUCCESS)
    {
        // bOverwritePrevious may not be set on a server that is using the API to add to the list
        if (!bOverwritePrevious)
            return u8Status;
        // If its a "duplicate" return now and silently discard it.  Duplicate must have same issuer ID.
        if (psPreviousPublishPriceRecord != NULL &&
            psPreviousPublishPriceRecord->sPublishPriceCmdPayload.u32IssuerEventId == psPublishPriceCmdPayload->u32IssuerEventId &&
            psPreviousPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime == psPublishPriceCmdPayload->u32StartTime &&
            (psPreviousPublishPriceRecord->sPublishPriceCmdPayload.u16DurationInMinutes == psPublishPriceCmdPayload->u16DurationInMinutes ||
              E_SE_PRICE_DURATION_UNTIL_CHANGED== psPublishPriceCmdPayload->u16DurationInMinutes))
        {
            return E_SE_PRICE_DUPLICATE;
        }
        // Incoming event with start of now - need to compare with first entry in the list.
        // This could be next or prev depending on what the first entry in the list was.
        // duration may be adjusted down
        if (psPublishPriceCmdPayload->u32StartTime == 0)
        {
            tsSE_PricePublishPriceRecord *psHeadRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lPriceAllocList );
            if (psHeadRecord != NULL &&
                psHeadRecord->sPublishPriceCmdPayload.u32IssuerEventId == psPublishPriceCmdPayload->u32IssuerEventId &&
                (psHeadRecord->sPublishPriceCmdPayload.u16DurationInMinutes >= psPublishPriceCmdPayload->u16DurationInMinutes ||
                E_SE_PRICE_DURATION_UNTIL_CHANGED == psPublishPriceCmdPayload->u16DurationInMinutes))

            {
                return E_SE_PRICE_DUPLICATE;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////
    // Delete all overlapping events
    ////////////////////////////////////////////////////////////////////////
    // Overlap allowed - Newer issuer ID always removes ALL overlaps with older ID.
    // Remove any overlap with previous event
    u8Status = eSE_CheckForTableOverlap(psPriceCustomDataStructure, NULL, psPreviousPublishPriceRecord, psPublishPriceCmdPayload->u32StartTime, psPublishPriceCmdPayload->u16DurationInMinutes);
    if(u8Status != E_ZCL_SUCCESS)
    {
        uint32 u32CurrentTime = u32ZCL_GetUTCTime();
        // Check new-ness.  If the event ID of the new item is not newer, this is an error and no delete will be done.
        if(psPreviousPublishPriceRecord->sPublishPriceCmdPayload.u32IssuerEventId >= psPublishPriceCmdPayload->u32IssuerEventId)
            return E_SE_PRICE_DATA_OLD;

        // Special rule if the event to be deleted is currently running and the incoming overlapping event is
        // in the future.  The current event shouldn't be deleted  but its time adjusted to end when the new event begins.
        if (u32CurrentTime >= psPreviousPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime &&
            u32CurrentTime < psPublishPriceCmdPayload->u32StartTime)
        {
            psPreviousPublishPriceRecord->sPublishPriceCmdPayload.u16DurationInMinutes = E_SE_PRICE_DURATION_UNTIL_CHANGED;
        }
        else
        {
            // Delete the previous record
            psDLISTremove(&psPriceCustomDataStructure->lPriceAllocList, (DNODE *)psPreviousPublishPriceRecord);
            vDLISTaddToHead(&psPriceCustomDataStructure->lPriceDeAllocList, (DNODE *)psPreviousPublishPriceRecord);
        }
    }

    // Now remove any overlapping following nodes - may be more than one.  psNextPublishPriceRecord is kept up to date
    do
    {
        DNODE *pTempNode;
        // Check for overlap returns success if both next and prev are NULL.
        u8Status = eSE_CheckForTableOverlap(psPriceCustomDataStructure, psNextPublishPriceRecord, NULL, psPublishPriceCmdPayload->u32StartTime, psPublishPriceCmdPayload->u16DurationInMinutes);
        if(u8Status != E_ZCL_SUCCESS)
        {
            // We fail if we find any existing events that have a newer or equall issuer ID
            // This could leave us having already deleted some older overlapping events and then
            // not adding the new one.  We don't know the server's intention in this case,
            // so doing this is as valid as checking and aborting with the list intact.
            if(psNextPublishPriceRecord->sPublishPriceCmdPayload.u32IssuerEventId >= psPublishPriceCmdPayload->u32IssuerEventId)
                return E_SE_PRICE_DATA_OLD;

            // Delete and move next pointer on
            pTempNode = (DNODE *)(psNextPublishPriceRecord);
            psNextPublishPriceRecord = (tsSE_PricePublishPriceRecord*)(pTempNode->psNext);
            psDLISTremove(&psPriceCustomDataStructure->lPriceAllocList, pTempNode);
            vDLISTaddToHead(&psPriceCustomDataStructure->lPriceDeAllocList, pTempNode);
        }
    }
    while(u8Status != E_ZCL_SUCCESS);

    ////////////////////////////////////////////////////////////////////////
    // Allocate a buffer for the new item
    ////////////////////////////////////////////////////////////////////////

    // we need to get a list item from the dealloc list
    // If the dealloc list is empty, we may be able to delete the last item on the list then make a recursive call to insert the item.
    psNewPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lPriceDeAllocList);
    if(psNewPublishPriceRecord==NULL)
    {
        // get tail of alloc list
        psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetTail( &psPriceCustomDataStructure->lPriceAllocList );
        // see if we can fit the new table in if has a start time earlier than the last entry in the list
        // Time takes priority over issuer ID on a full list.
        if(psPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime < psPublishPriceCmdPayload->u32StartTime)
        {
            return E_ZCL_ERR_INSUFFICIENT_SPACE;
        }
        // we can (possibly) fit it
        // remove tail from alloc and put it back on the free list
        psDLISTremove(&psPriceCustomDataStructure->lPriceAllocList, (DNODE *)psPublishPriceRecord);
        vDLISTaddToHead(&psPriceCustomDataStructure->lPriceDeAllocList, (DNODE *)psPublishPriceRecord);
        // try to add again
        u8Status = eSE_AddPriceEntryUsingPointer(psPriceCustomDataStructure, bOverwritePrevious, psPublishPriceCmdPayload);
        if(u8Status != E_ZCL_SUCCESS)
        {
            // restore last table back
            psDLISTremove(&psPriceCustomDataStructure->lPriceDeAllocList, (DNODE *)psPublishPriceRecord);
            vDLISTaddToTail(&psPriceCustomDataStructure->lPriceAllocList, (DNODE *)psPublishPriceRecord);
            return u8Status;
        }

        return E_ZCL_SUCCESS;
    }

    ////////////////////////////////////////////////////////////////////////
    // Add the new buffer to the list
    ////////////////////////////////////////////////////////////////////////


    // When we get to here, the list has been cleaned of overlap and a free item has been allocated for us.
    // What could possibly go wrong.....

    // remove from dealloc list
    psDLISTremove(&psPriceCustomDataStructure->lPriceDeAllocList, (DNODE *)psNewPublishPriceRecord);

    // add to alloc list
    if(psNextPublishPriceRecord==NULL)
    {
        // add to tail
        vDLISTaddToTail(&psPriceCustomDataStructure->lPriceAllocList, (DNODE *)psNewPublishPriceRecord);
    }
    else
    {
        vDLISTinsertBefore(&psPriceCustomDataStructure->lPriceAllocList, (DNODE *)psNextPublishPriceRecord, (DNODE *)psNewPublishPriceRecord);
    }

    vCopyRecordIntoTable(&psNewPublishPriceRecord->sPublishPriceCmdPayload, psPublishPriceCmdPayload);

    // If we have just inserted a start time of now - update the Arrived fields
    if (psNewPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime == 0)
    {
        psPriceCustomDataStructure->u32ArrivalTimeOfStartNowEntry     = u32ZCL_GetUTCTime();
        psPriceCustomDataStructure->u16ArrivalDurationOfStartNowEntry = psNewPublishPriceRecord->sPublishPriceCmdPayload.u16DurationInMinutes;
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
 ** checks to see if tabnle times overlap
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsSE_PriceCustomDataStructure     *psPriceCustomDataStructure,  original start and duration of list event with start now
 ** tsSE_PricePublishPriceRecord      *psNextPublishPriceRecord     table data
 ** tsSE_PricePublishPriceRecord      *psPreviousPublishPriceRecord table data
 ** uint32                     u32StartTime                 start time
 ** uint16                     u16DurationInMinutes         duration
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PRIVATE  teSE_PriceStatus eSE_CheckForTableOverlap(
                tsSE_PriceCustomDataStructure      *psPriceCustomDataStructure,
                tsSE_PricePublishPriceRecord       *psNextPublishPriceRecord,
                tsSE_PricePublishPriceRecord       *psPreviousPublishPriceRecord,
                uint32                      u32StartTime,
                uint16                      u16DurationInMinutes)
{
    // check next - if not at end of list
    if (psNextPublishPriceRecord !=NULL)
    {
        uint32 u32AdjustedStartTime =  (u32StartTime == 0) ? u32ZCL_GetUTCTime() : u32StartTime;
        if (u16DurationInMinutes == E_SE_PRICE_DURATION_UNTIL_CHANGED)
        {
            // Don't allow two events at same time but even one second is allowed
            // This could be a sneaky way to get events of duration less than a minute.
            if (u32AdjustedStartTime + 1 > psNextPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime)
            {
                return(E_SE_PRICE_OVERLAP);
            }
        }
        else
        {
            // Only adjust the incoming - if the next is the start of the list must be comparing zero with zero so will overlap
            if (u32AdjustedStartTime + u16DurationInMinutes*60 > psNextPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime)
            {
                return(E_SE_PRICE_OVERLAP);
            }
        }
    }

    // check previous - if not at start of list
    if (psPreviousPublishPriceRecord !=NULL)
    {
        // Make allowance for the first item in the list having the special start time of zero
        // Leave the incoming item at its normal time - if its zero it will always overlap an earlier item
        uint32 u32AdjustedPreviousStartTime = psPreviousPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime;
        uint16 u16AdjustedPreviousDuration  = psPreviousPublishPriceRecord->sPublishPriceCmdPayload.u16DurationInMinutes;
        if (u32AdjustedPreviousStartTime == 0)
        {
            // Check the start time against the original arrival parameters.  If we try to use the current duration
            // we are likely to run into rounding problems with the duration in minutes and the times in seconds.
            u32AdjustedPreviousStartTime = psPriceCustomDataStructure->u32ArrivalTimeOfStartNowEntry;
            u16AdjustedPreviousDuration  = psPriceCustomDataStructure->u16ArrivalDurationOfStartNowEntry;
        }
        if (u16AdjustedPreviousDuration == E_SE_PRICE_DURATION_UNTIL_CHANGED)
        {
            if (u32AdjustedPreviousStartTime + 1 > u32StartTime)
            {
                return(E_SE_PRICE_OVERLAP);
            }
        }
        else
        {
            if (u32AdjustedPreviousStartTime + u16AdjustedPreviousDuration*60 > u32StartTime)
            {
                return(E_SE_PRICE_OVERLAP);
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
 ** PARAMETERS:             Name              Usage
 ** void                    *pvSearchParam    value to search on
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
        &(((tsSE_PricePublishPriceRecord *)psNodeUnderTest)->sPublishPriceCmdPayload.u32StartTime),
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
 ** NAME:       eSE_FindPriceCluster
 **
 ** DESCRIPTION:
 ** Search helper function
 **
 ** PARAMETERS:                     Name                        Usage
 ** uint8                           u8SourceEndPointId          Source EP Id
 ** bool_t                          bIsServer                   Is server
 ** tsZCL_EndPointDefinition    **ppsEndPointDefinition         EP
 ** tsZCL_ClusterInstance       **ppsClusterInstance            Cluster
 ** tsSE_PriceCustomDataStructure  **ppsPriceCustomDataStructure   Price data
 **
 ** RETURN:
 ** 0 - Success, -1 Fail
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_FindPriceCluster(
                                uint8                           u8SourceEndPointId,
                                bool_t                          bIsServer,
                                tsZCL_EndPointDefinition      **ppsEndPointDefinition,
                                tsZCL_ClusterInstance         **ppsClusterInstance,
                                tsSE_PriceCustomDataStructure    **ppsPriceCustomDataStructure)
{
    // check EP is registered and cluster is present in the send device
    if(eZCL_SearchForEPentry(u8SourceEndPointId, ppsEndPointDefinition) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    if(eZCL_SearchForClusterEntry(u8SourceEndPointId, SE_CLUSTER_ID_PRICE, bIsServer, ppsClusterInstance) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
    }

    // initialise custom data pointer
    *ppsPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)(*ppsClusterInstance)->pvEndPointCustomStructPtr;
    if(*ppsPriceCustomDataStructure==NULL)
    {
        return(E_SE_PRICE_TABLE_NOT_FOUND);
    }

    return(E_ZCL_SUCCESS);
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

PRIVATE  void vCopyRecordIntoTable(tsSE_PricePublishPriceCmdPayload *psTablePublishPriceCmdPayload, tsSE_PricePublishPriceCmdPayload *psPublishPriceCmdPayload)
{

    tsZCL_OctetString sRateLabel;

    // take local copy of Table string
    sRateLabel = psTablePublishPriceCmdPayload->sRateLabel;
    // copy the whole structure - this will destroy the string structure
    memcpy(psTablePublishPriceCmdPayload, psPublishPriceCmdPayload, sizeof(tsSE_PricePublishPriceCmdPayload));
    // add original string data back in
    psTablePublishPriceCmdPayload->sRateLabel = sRateLabel;
    // write the new string in - safe copy use table max length.  Truncate to max length of SE string.
    memcpy(psTablePublishPriceCmdPayload->sRateLabel.pu8Data, psPublishPriceCmdPayload->sRateLabel.pu8Data, sRateLabel.u8MaxLength);
    // string length - input string length truncated to table max length
    if (psPublishPriceCmdPayload->sRateLabel.u8Length > sRateLabel.u8MaxLength)
        psTablePublishPriceCmdPayload->sRateLabel.u8Length = sRateLabel.u8MaxLength;
    else
        psTablePublishPriceCmdPayload->sRateLabel.u8Length = psPublishPriceCmdPayload->sRateLabel.u8Length;

}
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

