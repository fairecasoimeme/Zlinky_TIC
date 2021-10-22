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
 * COMPONENT:          PriceScheduler.c
 *
 * DESCRIPTION:        Update list of price events as time changes
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

PRIVATE bool_t boExpiredCheck(tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            tsSE_PricePublishPriceRecord *psPublishPriceRecord,
                            uint32 u32UTCTime);

#ifdef PRICE_CALORIFIC_VALUE
PRIVATE teSE_PriceStatus eSE_CalorificValueSchedulerUpdate(
                                uint8    u8SourceEndPointId,
                                bool_t  bIsServer,
                                uint32  u32CurrentTime);

#endif

#ifdef PRICE_CONVERSION_FACTOR
PRIVATE teSE_PriceStatus eSE_ConversionFactorSchedulerUpdate(
                                uint8   u8SourceEndPointId,
                                bool_t  bIsServer,
                                uint32  u32CurrentTime);
#endif

PRIVATE teSE_PriceStatus eSE_PriceSchedulerUpdate(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32CurrentTime);

#ifdef BLOCK_CHARGING
PRIVATE bool_t boBlockPeriodExpiredCheck(tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                        tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord,
                        uint32 u32UTCTime);



PRIVATE teSE_PriceStatus eSE_BlockPeriodSchedulerUpdate(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32CurrentTime);

#endif /* BLOCK_CHARGING */

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       vSE_PriceTimerClickCallback
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

PUBLIC  void vSE_PriceTimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent)
{
    int i;
    uint8 u8NumberOfendpoints;

    u8NumberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();

    // find price clusters on each EP - if any
    for(i=0; i<u8NumberOfendpoints; i++)
    {
        // deliver time to any EP-server/client
        eSE_PriceSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i), TRUE, psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
        eSE_PriceSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i), FALSE, psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);

#ifdef BLOCK_CHARGING
        // deliver time to any EP-server/client
        eSE_BlockPeriodSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i), TRUE, psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
        eSE_BlockPeriodSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i), FALSE, psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
#endif /* BLOCK_CHARGING */

#ifdef  PRICE_CONVERSION_FACTOR
        eSE_ConversionFactorSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i),TRUE,psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
        eSE_ConversionFactorSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i),FALSE,psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
#endif

#ifdef PRICE_CALORIFIC_VALUE
        eSE_CalorificValueSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i),TRUE,psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
        eSE_CalorificValueSchedulerUpdate(u8ZCL_GetEPIdFromIndex(i),FALSE,psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
#endif


    }

}

/****************************************************************************
 **
 ** NAME:       bSE_PriceRibUpdateCurrentPrice
 **
 ** DESCRIPTION:
 ** In RIB mode, the price in the head publish price is kept in sync with the
 ** prices in the attributes.
 **
 ** PARAMETERS:                          Name                            Usage
 **    tsSE_PricePublishPriceRecord    *psPublishPriceRecord;            Record to check
 ** uint32                      u32UTCTime                  Current time
 ** RETURN:
 ** TRUE if the price has been updated.
 **
 ****************************************************************************/

#ifdef SE_PRICE_SERVER_AUTO_UPDATES_PRICE_FROM_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED
PRIVATE  bool_t bSE_PriceRibUpdateCurrentPrice(
            tsSE_PriceCustomDataStructure  *psPriceCustomDataStructure,
            tsSE_PricePublishPriceCmdPayload *psPublishPriceCmdPayload,
            uint32 u32UTCTime)
{
    uint64 u64ConsumptionScaledAsThreshold;
    uint32 u32Price;
    int i;

    // Only update if the price is active.  Not currently keeping expired prices so no need to check for that
    if (psPublishPriceCmdPayload == NULL ||
        u32UTCTime < psPublishPriceCmdPayload->u32StartTime)
    {
        return FALSE;
    }

    if (psPriceCustomDataStructure->psPriceAttributes == NULL ||
        psPriceCustomDataStructure->pu48CurrentBlockPeriodConsumptionDelivered == NULL ||
        psPriceCustomDataStructure->pu24MeteringMultiplier == NULL ||
        psPriceCustomDataStructure->pu24MeteringDivisor == NULL)
    {
        // If not initialised correctly, give up.  Difficult to indicate error back to user app as in the middle of scheduler
        return FALSE;
    }

    if (psPublishPriceCmdPayload->u8NumberOfBlockThresholds < 1 || psPublishPriceCmdPayload->u8NumberOfBlockThresholds > CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT)
    {
        return FALSE;
    }

    // Scale the metering consumtion value to the same factor as the thresholds are scaled by
    u64ConsumptionScaledAsThreshold = *psPriceCustomDataStructure->pu48CurrentBlockPeriodConsumptionDelivered;
    u64ConsumptionScaledAsThreshold = u64ConsumptionScaledAsThreshold *
        psPriceCustomDataStructure->psPriceAttributes->u24ThresholdDivisor *  *psPriceCustomDataStructure->pu24MeteringMultiplier;
    u64ConsumptionScaledAsThreshold = u64ConsumptionScaledAsThreshold /
        (psPriceCustomDataStructure->psPriceAttributes->u24ThresholdMultiplier * *psPriceCustomDataStructure->pu24MeteringDivisor);


    // Loop rouund the thresholds comparing with the consumption this block period
    for(i=0; i<psPublishPriceCmdPayload->u8NumberOfBlockThresholds; i++)
    {
        uint64 u64Threshold = *(&psPriceCustomDataStructure->psPriceAttributes->au48BlockThreshold[0]
            + i *
            (&psPriceCustomDataStructure->psPriceAttributes->au48BlockThreshold[1] - &psPriceCustomDataStructure->psPriceAttributes->au48BlockThreshold[0]));

        if (u64ConsumptionScaledAsThreshold < u64Threshold)
        {
            break;
        }
    }

    u32Price = *(&psPriceCustomDataStructure->psPriceAttributes->au32NoTierBlockPrice[0]
    + i *
    (&psPriceCustomDataStructure->psPriceAttributes->au32NoTierBlockPrice[1] - &psPriceCustomDataStructure->psPriceAttributes->au32NoTierBlockPrice[0]));


    if (u32Price == psPublishPriceCmdPayload->u32Price)
    {
        return FALSE;
    }

    psPublishPriceCmdPayload->u32Price = u32Price;
    return TRUE;
}
#endif

/****************************************************************************
 **
 ** NAME:       eSE_PriceSchedulerUpdate
 **
 ** DESCRIPTION:
 ** Updates the Price Data Table - based on current time
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32UTCTime                  Current time
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PriceSchedulerUpdate(uint8 u8SourceEndPointId, bool_t bIsServer, uint32 u32UTCTime)
{
    tsPrice_Common *psPrice_Common;

    tsSE_PricePublishPriceRecord *psPublishPriceRecord;
    tsSE_PricePublishPriceRecord *psPublishPriceRecordCopy;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    bool_t bHaveDeleted = FALSE;  // Set if ANY item in the list has been deleted
    bool_t boHeadHasBecomeActive = FALSE;
    bool_t bDeletePriceEvent = FALSE;

    tsZCL_Address     sDestinationAddress;
    uint8             u8TransactionSequenceNumber;
    bool_t bRibPriceChanged = FALSE;

    // error check via EP number
    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    // initialise pointer
    psPrice_Common = &((tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sPrice_Common;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // get first resource
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lPriceAllocList);

    // If first entry in list has special start time of zero meaning now, update the duration unless duration is until changed
    if (psPublishPriceRecord &&
        psPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime == 0 &&
        psPublishPriceRecord->sPublishPriceCmdPayload.u16DurationInMinutes != E_SE_PRICE_DURATION_UNTIL_CHANGED)
    {
        int32 i32NewDuration = psPriceCustomDataStructure->u16ArrivalDurationOfStartNowEntry -
            (u32UTCTime - psPriceCustomDataStructure->u32ArrivalTimeOfStartNowEntry) / 60;
        if (i32NewDuration >= 0)
            psPublishPriceRecord->sPublishPriceCmdPayload.u16DurationInMinutes = (uint16)(i32NewDuration);
    }

    // search
    while(psPublishPriceRecord != NULL)
    {
        // check whether timer entry has fired
        bool_t boDeleteThisEntry = boExpiredCheck(psPriceCustomDataStructure, psPublishPriceRecord, u32UTCTime);
        bHaveDeleted |= boDeleteThisEntry;
        psPublishPriceRecordCopy = psPublishPriceRecord;
        // get next list member
        psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetNext((DNODE *)psPublishPriceRecord);
        if(boDeleteThisEntry)
        {
            // If keeping expired prices, update bSE_PriceRibUpdateCurrentPrice to check for expired.
            // May have to update other checks as well.
            // remove it from alloc  list
            psDLISTremove(&psPriceCustomDataStructure->lPriceAllocList, (DNODE *)psPublishPriceRecordCopy);
            // add to free list
            vDLISTaddToTail(&psPriceCustomDataStructure->lPriceDeAllocList, (DNODE *)psPublishPriceRecordCopy);

            bDeletePriceEvent = TRUE;
        }
    }

    // re-get the head
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lPriceAllocList);


#ifdef SE_PRICE_SERVER_AUTO_UPDATES_PRICE_FROM_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED
    if (psPublishPriceRecord)
    {
        bRibPriceChanged = bSE_PriceRibUpdateCurrentPrice(psPriceCustomDataStructure,
            &psPublishPriceRecord->sPublishPriceCmdPayload,
            u32UTCTime);
    }
#endif

    // Check if head has become active on this time tick
    if(psPublishPriceRecord && (psPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime == u32UTCTime || bRibPriceChanged))
    {
        boHeadHasBecomeActive = TRUE;

        /* Send the Publish Price Cmd to bound address. It is only for server */
        if(bIsServer)
    {
            u8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
            #ifdef CLD_PRICE_BOUND_TX_WITH_APS_ACK_DISABLED
                sDestinationAddress.eAddressMode = E_ZCL_AM_BOUND_NO_ACK;
            #else
                sDestinationAddress.eAddressMode = E_ZCL_AM_BOUND;
            #endif 
            eSE_PricePublishPriceSend(u8SourceEndPointId,
                                     0,    /* Not considered for bound address mode */
                                     &sDestinationAddress,
                                     &u8TransactionSequenceNumber,
                                     &psPublishPriceRecord->sPublishPriceCmdPayload);
        }
    }

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, 0, 0, psEndPointDefinition->u8EndPointNumber);
    psPrice_Common->sPriceCallBackMessage.u32CurrentTime = u32UTCTime;

    // new price table has come into play - inform the user
    if(psPublishPriceRecord && (bHaveDeleted || boHeadHasBecomeActive))
    {

        // Price callback
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_TABLE_ACTIVE;
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableTimeEvent.ePriceStatus = E_ZCL_SUCCESS;
        // If last record deleted - can't check time but do generate a not yet active event
        if(!psPublishPriceRecord || psPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime > u32UTCTime)
        {
            psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableTimeEvent.ePriceStatus = E_SE_PRICE_TABLE_NOT_YET_ACTIVE;
        }
        // fill in free entries
        eSE_GetNumberOfFreePriceTableEntries(psEndPointDefinition, psPriceCustomDataStructure, &psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableTimeEvent.u8NumberOfEntriesFree);

        // call user
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }
    else if(bDeletePriceEvent)
    {
        // Price callback
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_NO_PRICE_TABLES;
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableTimeEvent.ePriceStatus = E_SE_PRICE_NO_TABLES;

        // fill in free entries
        eSE_GetNumberOfFreePriceTableEntries(psEndPointDefinition, psPriceCustomDataStructure, &psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableTimeEvent.u8NumberOfEntriesFree);

        // call user
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}

#ifdef PRICE_CALORIFIC_VALUE

PRIVATE  bool_t boCalorificValueExpiredCheck(tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            tsSE_PriceCalorificValueRecord *psPublishCalorificValueRecord,
                            uint32 u32UTCTime)
{

    tsSE_PricePublishCalorificValueCmdPayload *psPublishCalorificValueCmdPayload = &(psPublishCalorificValueRecord->sPublishCalorificValueCmdPayload);
    uint32 u32StartTime = psPublishCalorificValueCmdPayload->u32StartTime;

    if(u32StartTime <= u32UTCTime)
    {
        return(TRUE);
    }

    return FALSE;

}


/****************************************************************************
 **
 ** NAME:       eSE_CalorificValueSchedulerUpdate
 **
 ** DESCRIPTION:
 ** Updates the Price Data Table - based on current time
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32UTCTime                  Current time
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PRIVATE  teSE_PriceStatus eSE_CalorificValueSchedulerUpdate(
                                uint8    u8SourceEndPointId,
                                bool_t  bIsServer,
                                uint32  u32CurrentTime)
{
    tsPrice_Common *psPrice_Common;
    bool_t                             bNewEvent = FALSE;
    tsSE_PriceCalorificValueRecord *psPublishCalorificValueRecord;
    tsSE_PriceCalorificValueRecord *psPublishCalorificValueRecordCopy=NULL;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;

    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    bool_t boDeleteThisEntry = FALSE;

    uint32 u32CalorificValue;
    uint8 e8CalorificValueUnit, b8CalorificValueTrailingDigit;

    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    psPrice_Common = &((tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sPrice_Common;

    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lCalorificValueAllocList);

    while(psPublishCalorificValueRecord != NULL)
    {
        boDeleteThisEntry = boCalorificValueExpiredCheck(psPriceCustomDataStructure,psPublishCalorificValueRecord, u32CurrentTime);

        if(boDeleteThisEntry && psPublishCalorificValueRecordCopy != NULL)
        {
                 psDLISTremove(&psPriceCustomDataStructure->lCalorificValueAllocList, (DNODE *)psPublishCalorificValueRecordCopy);
                 vDLISTaddToTail(&psPriceCustomDataStructure->lCalorificValueDeAllocList, (DNODE *)psPublishCalorificValueRecordCopy);
                 bNewEvent = TRUE;
        }
        if(boDeleteThisEntry)
                psPublishCalorificValueRecordCopy = psPublishCalorificValueRecord;

        psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetNext((DNODE *)psPublishCalorificValueRecord);

    }

    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, 0, 0, psEndPointDefinition->u8EndPointNumber);

    psPrice_Common->sPriceCallBackMessage.u32CurrentTime = u32CurrentTime;

    eSE_GetNumberOfFreeCalorificValueTableEntries(psEndPointDefinition, psPriceCustomDataStructure,
              &psPrice_Common->sPriceCallBackMessage.uMessage.sCalorificValueTableTimeEvent.u8NumberOfEntriesFree);

    eZCL_GetLocalAttributeValue(E_CLD_P_ATTR_CALORIFIC_VALUE,FALSE,FALSE,psEndPointDefinition,psClusterInstance,&u32CalorificValue);
    eZCL_GetLocalAttributeValue(E_CLD_P_ATTR_CALORIFIC_VALUE_UNIT, FALSE,FALSE,psEndPointDefinition,psClusterInstance,&e8CalorificValueUnit);
    eZCL_GetLocalAttributeValue(E_CLD_P_ATTR_CALORIFIC_VALUE_TRAILING_DIGIT,FALSE,FALSE,psEndPointDefinition,psClusterInstance,&b8CalorificValueTrailingDigit);

    if( boDeleteThisEntry &&  u32CalorificValue     != (psPublishCalorificValueRecordCopy->sPublishCalorificValueCmdPayload).u32CalorificValue &&
        e8CalorificValueUnit                      != (psPublishCalorificValueRecordCopy->sPublishCalorificValueCmdPayload).e8CalorificValueUnit &&
        b8CalorificValueTrailingDigit             != (psPublishCalorificValueRecordCopy->sPublishCalorificValueCmdPayload).b8CalorificValueTrailingDigit )
                bNewEvent = TRUE;

    if(psPublishCalorificValueRecordCopy != NULL && ( bNewEvent) )
    {

            eZCL_SetLocalAttributeValue(E_CLD_P_ATTR_CALORIFIC_VALUE,FALSE,FALSE,psEndPointDefinition,psClusterInstance,
            &psPublishCalorificValueRecordCopy->sPublishCalorificValueCmdPayload.u32CalorificValue);

            eZCL_SetLocalAttributeValue(E_CLD_P_ATTR_CALORIFIC_VALUE_UNIT, FALSE,FALSE,psEndPointDefinition,psClusterInstance,
            &psPublishCalorificValueRecordCopy->sPublishCalorificValueCmdPayload.e8CalorificValueUnit);

            eZCL_SetLocalAttributeValue(E_CLD_P_ATTR_CALORIFIC_VALUE_TRAILING_DIGIT,FALSE,FALSE,psEndPointDefinition,psClusterInstance,
            &psPublishCalorificValueRecordCopy->sPublishCalorificValueCmdPayload.b8CalorificValueTrailingDigit);

            psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_CALORIFIC_VALUE_TABLE_ACTIVE;
            psPrice_Common->sPriceCallBackMessage.uMessage.sCalorificValueTableTimeEvent.eCalorificValueStatus = E_ZCL_SUCCESS;
            psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);



    }

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}
#endif /* CALORIFIC VALUE */



#ifdef PRICE_CONVERSION_FACTOR

/****************************************************************************
 **
 ** NAME:       boExpiredCheck
 **
 ** DESCRIPTION:
 ** Checks for expired timers - calls registered user c/b's
 **
 ** PARAMETERS:                      Name                                    Usage
 ** tsSE_PriceCustomDataStructure    *psPriceCustomDataStructure         Holds arrival time when start time is now
 ** tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecord   Price Record to check
 ** uint32                            u32UTCTime                         Time Value
 **
 ** RETURN:
 ** 1 - Delete 0 - Don't
 **
 ****************************************************************************/

PRIVATE  bool_t boConversionFactorCheckActive(tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecord,
                            uint32 u32UTCTime)
{

    tsSE_PricePublishConversionCmdPayload *psPublishConversionCmdPayload = &(psPublishConversionFactorRecord->sPublishConversionCmdPayload);
    uint32 u32StartTime = psPublishConversionCmdPayload->u32StartTime;

    if(u32StartTime <= u32UTCTime)
    {
        return(TRUE);
    }

    return FALSE;

}


/****************************************************************************
 **
 ** NAME:       eSE_ConversionFactorSchedulerUpdate
 **
 ** DESCRIPTION:
 ** Updates the Price Data Table - based on current time
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32UTCTime                  Current time
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_ConversionFactorSchedulerUpdate(
                                uint8    u8SourceEndPointId,
                                bool_t  bIsServer,
                                uint32  u32CurrentTime)
{
    tsPrice_Common *psPrice_Common;
    bool_t bNewEvent = FALSE;
    tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecord;
    tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecordCopy = NULL;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    uint32 u32ConversionFactor;
    uint8  b8ConversionFactorTrailingDigit;

    bool_t boDeleteThisEntry = FALSE;

    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    psPrice_Common = &((tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sPrice_Common;
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lConversionFactorAllocList);

    while(psPublishConversionFactorRecord != NULL)
    {
        boDeleteThisEntry = boConversionFactorCheckActive(psPriceCustomDataStructure,psPublishConversionFactorRecord, u32CurrentTime);

        if(boDeleteThisEntry && psPublishConversionFactorRecordCopy != NULL)
        {
                psDLISTremove(&psPriceCustomDataStructure->lConversionFactorAllocList, (DNODE *)psPublishConversionFactorRecordCopy);
                vDLISTaddToTail(&psPriceCustomDataStructure->lConversionFactorDeAllocList, (DNODE *)psPublishConversionFactorRecordCopy);
                bNewEvent = TRUE;

        }
        if(boDeleteThisEntry)
            psPublishConversionFactorRecordCopy = psPublishConversionFactorRecord;

        psPublishConversionFactorRecord =  (tsSE_PriceConversionFactorRecord *)psDLISTgetNext((DNODE *)psPublishConversionFactorRecord);

    }

    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, 0, 0, psEndPointDefinition->u8EndPointNumber);
    psPrice_Common->sPriceCallBackMessage.u32CurrentTime = u32CurrentTime;

    eSE_GetNumberOfFreeConversionFactorTableEntries(psEndPointDefinition, psPriceCustomDataStructure,
                 &psPrice_Common->sPriceCallBackMessage.uMessage.sConversionFactorTableTimeEvent.u8NumberOfEntriesFree);

    // This case will only Arise when only one entry is there in the node
    eZCL_GetLocalAttributeValue(E_CLD_P_ATTR_CONVERSION_FACTOR,FALSE,FALSE,psEndPointDefinition,psClusterInstance, &u32ConversionFactor);
    eZCL_GetLocalAttributeValue(E_CLD_P_ATTR_CONVERSION_FACTOR_TRAILING_DIGIT,FALSE,FALSE,psEndPointDefinition,psClusterInstance, &b8ConversionFactorTrailingDigit);

    if(boDeleteThisEntry &&  u32ConversionFactor != (psPublishConversionFactorRecordCopy -> sPublishConversionCmdPayload).u32ConversionFactor &&
       b8ConversionFactorTrailingDigit              != (psPublishConversionFactorRecordCopy -> sPublishConversionCmdPayload).b8ConversionFactorTrailingDigit)
            bNewEvent = TRUE;

    if(psPublishConversionFactorRecordCopy != NULL && ( bNewEvent ) )
    {
            eZCL_SetLocalAttributeValue(E_CLD_P_ATTR_CONVERSION_FACTOR, FALSE,FALSE,psEndPointDefinition,psClusterInstance,
            &psPublishConversionFactorRecordCopy->sPublishConversionCmdPayload.u32ConversionFactor);

            eZCL_SetLocalAttributeValue(E_CLD_P_ATTR_CONVERSION_FACTOR_TRAILING_DIGIT, FALSE,FALSE,psEndPointDefinition,psClusterInstance,
            &psPublishConversionFactorRecordCopy->sPublishConversionCmdPayload.b8ConversionFactorTrailingDigit);

             psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_CONVERSION_FACTOR_TABLE_ACTIVE;
             psPrice_Common->sPriceCallBackMessage.uMessage.sConversionFactorTableTimeEvent.eConversionFactorStatus = E_ZCL_SUCCESS;
            psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);


    }

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}
#endif /* CONVERSION FACTOR */



#ifdef BLOCK_CHARGING
/****************************************************************************
 **
 ** NAME:       eSE_BlockPeriodSchedulerUpdate
 **
 ** DESCRIPTION:
 ** Updates the Price Data Table - based on current time
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32UTCTime                  Current time
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_BlockPeriodSchedulerUpdate(
                                uint8    u8SourceEndPointId,
                                bool_t  bIsServer,
                                uint32  u32CurrentTime)
{
    tsPrice_Common *psPrice_Common;

    tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord;
    tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecordCopy;

    uint8 u8FindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    bool_t bHaveDeleted = FALSE;  // Set if ANY item in the list has been deleted
    bool_t boHeadHasBecomeActive = FALSE;
    bool_t boDeleteThisEntry = FALSE;
    bool_t bDeleteBlockEvent = FALSE;

    // error check via EP number
    u8FindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);
    if(u8FindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return u8FindPriceClusterReturn;
    }

    // initialise pointer
    psPrice_Common = &((tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sPrice_Common;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // get first resource
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lBlockPeriodAllocList);

    // If first entry in list has special start time of zero meaning now, update the duration unless duration is until changed
    if (psPublishBlockPeriodRecord &&
        psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime == 0 &&
        psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins != E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED)
    {
        int32 i32NewDuration = psPriceCustomDataStructure->u32BlockPeriodArrivalDurationOfStartNowEntry -
            (u32CurrentTime - psPriceCustomDataStructure->u32BlockPeriodArrivalTimeOfStartNowEntry) / 60;
        if (i32NewDuration >= 0)
            psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins =
                (uint32)(i32NewDuration);
    }

    // search
    while(psPublishBlockPeriodRecord != NULL)
    {
        // check whether timer entry has fired
        boDeleteThisEntry = boBlockPeriodExpiredCheck(psPriceCustomDataStructure,
                                    psPublishBlockPeriodRecord, u32CurrentTime);
        bHaveDeleted |= boDeleteThisEntry;
        psPublishBlockPeriodRecordCopy = psPublishBlockPeriodRecord;
        // get next list member
        psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetNext((DNODE *)psPublishBlockPeriodRecord);
        if(boDeleteThisEntry)
        {
            uint8 u8BlockPeriodControl = psPublishBlockPeriodRecordCopy->sPublishBlockPeriodCmdPayload.b8BlockPeriodControl;
            /* check repeatable block or not  & extract b1 bit */
            if( (u8BlockPeriodControl != 0xFF) &&
                (u8BlockPeriodControl & 0x02) )
            {
                /* update start time field */
                if(psPublishBlockPeriodRecordCopy->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime == 0)
                {
                    psPublishBlockPeriodRecordCopy->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins =
                        psPriceCustomDataStructure->u32BlockPeriodArrivalDurationOfStartNowEntry;
                }
                else
                {
                    psPublishBlockPeriodRecordCopy->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime =
                        u32CurrentTime;
                }
            }
            else
            {
                // remove it from alloc  list
                psDLISTremove(&psPriceCustomDataStructure->lBlockPeriodAllocList, (DNODE *)psPublishBlockPeriodRecordCopy);
                // add to free list
                vDLISTaddToTail(&psPriceCustomDataStructure->lBlockPeriodDeAllocList, (DNODE *)psPublishBlockPeriodRecordCopy);

                bDeleteBlockEvent = TRUE;
            }
        }
    }

    // re-get the head
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lBlockPeriodAllocList);

    // Check if head has become active on this time tick
    if(psPublishBlockPeriodRecord && bHaveDeleted)
    {
        if(psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime == u32CurrentTime ||
                psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime == 0    )
        {
            boHeadHasBecomeActive = TRUE;
            /* When head is active, Update server block threshold values */
            if(bIsServer)
                eSE_PriceAddBlockThresholds(u8SourceEndPointId, &psPublishBlockPeriodRecord->sBlockThresholds);
        }
    }

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, 0, 0, psEndPointDefinition->u8EndPointNumber);
    psPrice_Common->sPriceCallBackMessage.u32CurrentTime = u32CurrentTime;

    // fill in free entries
    eSE_GetNumberOfFreeBlockPeriodTableEntries(psEndPointDefinition, psPriceCustomDataStructure,
                &psPrice_Common->sPriceCallBackMessage.uMessage.sBlockPeriodTableTimeEvent.u8NumberOfEntriesFree);

    // new block period table has come into play - inform the user
    if((bHaveDeleted && psPublishBlockPeriodRecord) || boHeadHasBecomeActive)
    {
        // Price callback
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_BLOCK_PERIOD_TABLE_ACTIVE;
        psPrice_Common->sPriceCallBackMessage.uMessage.sBlockPeriodTableTimeEvent.eBlockPeriodStatus = E_ZCL_SUCCESS;

        // If last record deleted - can't check time but do generate a not yet active event
        if(psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime != 0 &&
                psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime > u32CurrentTime)
        {
            psPrice_Common->sPriceCallBackMessage.uMessage.sBlockPeriodTableTimeEvent.eBlockPeriodStatus = E_SE_PRICE_BLOCK_PERIOD_TABLE_NOT_YET_ACTIVE;
        }

        // call user
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }
    else if(psPublishBlockPeriodRecord == NULL && bDeleteBlockEvent)
    {
        /* No events in the alloc list; inform user about this, user has to handle this situation */
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_NO_BLOCK_PERIOD_TABLES;
        psPrice_Common->sPriceCallBackMessage.uMessage.sBlockPeriodTableTimeEvent.eBlockPeriodStatus = E_SE_PRICE_NO_BLOCKS;

        // call user
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }


    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}
#endif /* BLOCK_CHARGING */

/****************************************************************************
 **
 ** NAME:       eSE_GetNumberOfFreePriceTableEntries
 **
 ** DESCRIPTION:
 ** gets number of free table entries
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** tsSE_PriceCustomDataStructure *psPriceCustomDataStructure  Price data structure
 ** uint8                      *pu8NumberOfEntries          Number of free entries
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_GetNumberOfFreePriceTableEntries(tsZCL_EndPointDefinition *psEndPointDefinition, tsSE_PriceCustomDataStructure *psPriceCustomDataStructure, uint8 *pu8NumberOfEntries)
{

    tsSE_PricePublishPriceRecord *psPublishPriceRecord;

    if((psEndPointDefinition==NULL) || (psPriceCustomDataStructure==NULL) || (pu8NumberOfEntries==NULL))
    {
        return(E_ZCL_FAIL);
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    *pu8NumberOfEntries = 0;
    // search through dealloc list
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lPriceDeAllocList);
    // search list to count
    while(psPublishPriceRecord!=NULL)
    {
        (*pu8NumberOfEntries)++;
        psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetNext((DNODE *)psPublishPriceRecord);
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}


#ifdef PRICE_CALORIFIC_VALUE
/***************************************************************************************************
 **
 ** NAME:       eSE_GetNumberOfFreeCalorificValueTableEntries
 **
 ** DESCRIPTION:
 ** gets number of free table entries
 **
 ** PARAMETERS:                             Name                            Usage
 ** tsZCL_EndPointDefinition         *psEndPointDefinition              EP definition
 ** tsSE_PriceCustomDataStructure    *psPriceCustomDataStructure      Price data structure
 ** uint8                            *pu8NumberOfEntries              Number of free entries
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ***************************************************************************************************/

PUBLIC  teSE_PriceStatus eSE_GetNumberOfFreeCalorificValueTableEntries(
                            tsZCL_EndPointDefinition *psEndPointDefinition,
                            tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            uint8 *pu8NumberOfEntries)
{

    tsSE_PriceCalorificValueRecord *psCalorificValueRecord;

    if((psEndPointDefinition==NULL) || (psPriceCustomDataStructure==NULL) || (pu8NumberOfEntries==NULL))
    {
        return(E_ZCL_FAIL);
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    *pu8NumberOfEntries = 0;
    // search through dealloc list
    psCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lCalorificValueDeAllocList);
    // search list to count

//    DBG_vPrintf(TRUE,"CAL_L1\n");
    while(psCalorificValueRecord != NULL)
    {
  //      DBG_vPrintf(TRUE,"Inside Loop\n");
        (*pu8NumberOfEntries)++;
    //    DBG_vPrintf(TRUE,"INSIDE Loop\n");
        psCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetNext((DNODE *)psCalorificValueRecord);
      //  DBG_vPrintf(TRUE,"Loop Inside\n");
    }

//    DBG_vPrintf(TRUE,"CAL_L2\n");
    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}
#endif /* PRICE_CALORIFIC_VALUE */

#ifdef PRICE_CONVERSION_FACTOR
/***************************************************************************************************
 **
 ** NAME:       eSE_GetNumberOfFreeConversionFactorTableEntries
 **
 ** DESCRIPTION:
 ** gets number of free table entries
 **
 ** PARAMETERS:                     Name                            Usage
 ** tsZCL_EndPointDefinition         *psEndPointDefinition              EP definition
 ** tsSE_PriceCustomDataStructure    *psPriceCustomDataStructure      Price data structure
 ** uint8                            *pu8NumberOfEntries              Number of free entries
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ***************************************************************************************************/

PUBLIC  teSE_PriceStatus eSE_GetNumberOfFreeConversionFactorTableEntries(
                            tsZCL_EndPointDefinition *psEndPointDefinition,
                            tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            uint8 *pu8NumberOfEntries)
{

    tsSE_PriceConversionFactorRecord *psPublishConversionFactorRecord;

    if((psEndPointDefinition==NULL) || (psPriceCustomDataStructure==NULL) || (pu8NumberOfEntries==NULL))
    {
        return(E_ZCL_FAIL);
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    *pu8NumberOfEntries = 0;
    // search through dealloc list
    psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lConversionFactorDeAllocList);
    // search list to count
    while(psPublishConversionFactorRecord != NULL)
    {
        (*pu8NumberOfEntries)++;
        psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetNext((DNODE *)psPublishConversionFactorRecord);
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}
#endif /* PRICE_CONVERSION_FACTOR */




#ifdef BLOCK_CHARGING
/****************************************************************************
 **
 ** NAME:       eSE_GetNumberOfFreeBlockPeriodTableEntries
 **
 ** DESCRIPTION:
 ** gets number of free table entries
 **
 ** PARAMETERS:                     Name                            Usage
 ** tsZCL_EndPointDefinition         *psEndPointDefinition              EP definition
 ** tsSE_PriceCustomDataStructure     *psPriceCustomDataStructure      Price data structure
 ** uint8                              *pu8NumberOfEntries              Number of free entries
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_GetNumberOfFreeBlockPeriodTableEntries(
                            tsZCL_EndPointDefinition *psEndPointDefinition,
                            tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            uint8 *pu8NumberOfEntries)
{

    tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord;

    if((psEndPointDefinition==NULL) || (psPriceCustomDataStructure==NULL) || (pu8NumberOfEntries==NULL))
    {
        return(E_ZCL_FAIL);
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    *pu8NumberOfEntries = 0;
    // search through dealloc list
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead(&psPriceCustomDataStructure->lBlockPeriodDeAllocList);
    // search list to count
    while(psPublishBlockPeriodRecord!=NULL)
    {
        (*pu8NumberOfEntries)++;
        psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetNext((DNODE *)psPublishBlockPeriodRecord);
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}
#endif /* BLOCK_CHARGING */

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       boExpiredCheck
 **
 ** DESCRIPTION:
 ** Checks for expired timers - calls registered user c/b's
 **
 ** PARAMETERS:                      Name                            Usage
 ** tsSE_PriceCustomDataStructure    *psPriceCustomDataStructure     Holds arrival time when start time is now
 ** tsSE_PricePublishPriceRecord     *psPublishPriceRecord           Price Record to check
 ** uint32                            u32UTCTime                     Time Value
 **
 ** RETURN:
 ** 1 - Delete 0 - Don't
 **
 ****************************************************************************/

PRIVATE  bool_t boExpiredCheck(tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            tsSE_PricePublishPriceRecord *psPublishPriceRecord,
                            uint32 u32UTCTime)
{
    tsSE_PricePublishPriceCmdPayload *psPublishPriceCmdPayload = &(psPublishPriceRecord->sPublishPriceCmdPayload);
    uint32 u32StartTime = psPublishPriceCmdPayload->u32StartTime;
    uint16 u16DurationInMinutes =  psPublishPriceCmdPayload->u16DurationInMinutes;

    // Special duration of until changed - check the next item in the list
    if (psPublishPriceCmdPayload->u16DurationInMinutes == E_SE_PRICE_DURATION_UNTIL_CHANGED)
    {
        if (psPublishPriceRecord->dllPriceNode.psNext == NULL)
        {
            // No next record - carry on until a new message arrives
            return FALSE;
        }
        psPublishPriceCmdPayload = &(((tsSE_PricePublishPriceRecord *)(psPublishPriceRecord->dllPriceNode.psNext))->sPublishPriceCmdPayload);
        if (psPublishPriceCmdPayload->u32StartTime <= u32UTCTime)
        {
            return TRUE;
        }

    }

    // Special start time of zero meaning now - test against the arrival time and duration when arrived.
    if(psPublishPriceCmdPayload->u32StartTime  == 0)
    {
        u32StartTime = psPriceCustomDataStructure->u32ArrivalTimeOfStartNowEntry;
        u16DurationInMinutes = psPriceCustomDataStructure->u16ArrivalDurationOfStartNowEntry;
    }

    if(u32StartTime + u16DurationInMinutes*60 <= u32UTCTime)
    {
        return(TRUE);
    }

    // no deletion required
    return FALSE;

}

#ifdef BLOCK_CHARGING
/****************************************************************************
 **
 ** NAME:       boBlockPeriodExpiredCheck
 **
 ** DESCRIPTION:
 ** Checks for expired timers - calls registered user c/b's
 **
 ** PARAMETERS:                          Name                            Usage
 ** tsSE_PriceCustomDataStructure        *psPriceCustomDataStructure     Holds arrival time when start time is now
 ** tsSE_PricePublishBlockPeriodRecord  *psPublishBlockPeriodRecord     Block Period Record to check
 ** uint32                                u32UTCTime                      Time Value
 **
 ** RETURN:
 ** 1 - Delete 0 - Don't
 **
 ****************************************************************************/

PRIVATE  bool_t boBlockPeriodExpiredCheck(tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord,
                            uint32 u32UTCTime)
{
    tsSE_PricePublishBlockPeriodCmdPayload *psPublishBlockPeriodCmdPayload =
                                &(psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload);
    uint32 u32StartTime = psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime;
    uint32 u32DurationInMinutes =  psPublishBlockPeriodCmdPayload->u32BlockPeriodDurationInMins;

    // Special duration of until changed - check the next item in the list
    if (u32DurationInMinutes == E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED)
    {
        if (psPublishBlockPeriodRecord->dllBlockPeriodNode.psNext == NULL)
        {
            // No next record - carry on until a new message arrives
            return FALSE;
        }
        /* Get Next record in the list */
        psPublishBlockPeriodCmdPayload =
            &(((tsSE_PricePublishBlockPeriodRecord *)(psPublishBlockPeriodRecord->dllBlockPeriodNode.psNext))->sPublishBlockPeriodCmdPayload);

        if (psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime <= u32UTCTime)
        {
            return TRUE;
        }

    }

    // Special start time of zero meaning now - test against the arrival time and duration when arrived.
    if(psPublishBlockPeriodCmdPayload->u32BlockPeriodStartTime  == 0)
    {
        u32StartTime = psPriceCustomDataStructure->u32BlockPeriodArrivalTimeOfStartNowEntry;
        u32DurationInMinutes = psPriceCustomDataStructure->u32BlockPeriodArrivalDurationOfStartNowEntry;
    }

    if(u32StartTime + u32DurationInMinutes*60 <= u32UTCTime)
    {
        return(TRUE);
    }

    // no deletion required
    return FALSE;

}
#endif /* BLOCK_CHARGING */
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

