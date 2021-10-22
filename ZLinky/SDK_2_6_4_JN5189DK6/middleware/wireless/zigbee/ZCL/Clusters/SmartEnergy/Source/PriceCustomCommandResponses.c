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
 * COMPONENT:          PriceCustomCommandResponse.c
 *
 * DESCRIPTION:        Respond to get current and scheduled price commands by sending publish price commands
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Price.h"
#include "Price_internal.h"

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
PRIVATE teSE_PriceStatus eSE_GeneratePublishPriceMessage(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsSE_PricePublishPriceCmdPayload    *psPublishPriceCmdPayload);

PRIVATE  void vSE_PublishPriceResponse(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8TransactionSequenceNumber,
                    tsSE_PricePublishPriceCmdPayload    *psPublishPriceCmdPayload);

#ifdef BLOCK_CHARGING
PRIVATE teSE_PriceStatus eSE_GeneratePublishBlockPeriodMessage(
                    ZPS_tsAfEvent                           *pZPSevent,
                    tsZCL_EndPointDefinition                *psEndPointDefinition,
                    tsZCL_ClusterInstance                   *psClusterInstance,
                    tsSE_PricePublishBlockPeriodCmdPayload  *psPublishBlockPeriodCmdPayload);

PRIVATE  void vSE_PublishBlockPeriodResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8TransactionSequenceNumber,
                    tsSE_PricePublishBlockPeriodCmdPayload *sPublishBlockPeriodCmdPayload);
#endif /* BLOCK_CHARGING */
#ifdef PRICE_CONVERSION_FACTOR
PRIVATE teSE_PriceStatus eSE_GeneratePublishConversionFactorMessage(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsSE_PricePublishConversionCmdPayload    *psPublishConversionCmdPayload);
#endif
#ifdef PRICE_CALORIFIC_VALUE
PRIVATE teSE_PriceStatus eSE_GeneratePublishCalorificValueMessage(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsSE_PricePublishCalorificValueCmdPayload    *psPublishCalorificCmdPayload);
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eSE_PriceRegisterPriceTimeServer
 **
 ** DESCRIPTION:
 ** Registers Price Time Server with ZCL
 **
 ** PARAMETERS:                         Name                          Usage
 ** none
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PriceRegisterPriceTimeServer(void)
{

    static bool_t bPriceTimerRegistered = FALSE;
    // add timer click function to ZCL
#ifndef OTA_UNIT_TEST_FRAMEWORK
    if(!bPriceTimerRegistered)
#endif
    {
        if(eZCL_TimerRegister(E_ZCL_TIMER_CLICK, 0, vSE_PriceTimerClickCallback)!= E_ZCL_SUCCESS)
        {
            return(E_ZCL_FAIL);
        }

        bPriceTimerRegistered = TRUE;
    }
    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eSE_HandleGetCurrentPrice
 **
 ** DESCRIPTION:
 ** Handles Price CLuster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC  void vSE_HandleGetCurrentPrice(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    tsPrice_Common *psPrice_Common;

    uint8 u8Status;
    uint8 u8TransactionSequenceNumber;
    teSE_PriceCommandOptions eSE_PriceCommandOptions;

    tsSE_PricePublishPriceRecord *psPublishPriceRecord;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;

    // get pricing option
    u8Status = eSE_GetCurrentPriceReceive(pZPSevent, &u8TransactionSequenceNumber, &eSE_PriceCommandOptions);

    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;
    // initialise pointer
    psPrice_Common = &psPriceCustomDataStructure->sPrice_Common;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    // Price callback
    psPrice_Common->sPriceCallBackMessage.u32CurrentTime = 0; // Time not relevant for server callback
    psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_GET_CURRENT_PRICE_RECEIVED;
    psPrice_Common->sPriceCallBackMessage.uMessage.ePriceCommandOptions = eSE_PriceCommandOptions;
    // call user
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

    // get head of alloc list - this is always the current price
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lPriceAllocList );

    if(u8Status != E_ZCL_SUCCESS ||   // Incoming message not parsed correctly
        psPublishPriceRecord==NULL || // No time at head of list
        psPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime > u32ZCL_GetUTCTime())  // Event not active yet
    {
        // reply with a default message
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // The message at the head of the list is valid for current time so send it out.
    u8Status = eSE_GeneratePublishPriceMessage(pZPSevent, psEndPointDefinition, psClusterInstance, &psPublishPriceRecord->sPublishPriceCmdPayload);
    if(u8Status != E_ZCL_SUCCESS)
    {
        // Can get error if time not synchronised
        // reply with a default message
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SOFTWARE_FAILURE);
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}


/****************************************************************************
 **
 ** NAME:       eSE_HandleGetSheduledPrices
 **
 ** DESCRIPTION:
 ** Handles Price CLuster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC  void vSE_HandleGetSheduledPrices(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    uint8 u8TransactionSequenceNumber;
    uint8 u8NumberOfEvents;
    uint32 u32StartTime, u32PriceEventStartTime;
    uint8 u8Status;
    uint8 u8NumberOfEventsSent;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsSE_PricePublishPriceRecord *psPublishPriceRecord;

    eSE_GetScheduledPricesReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &u32StartTime,
                    &u8NumberOfEvents);

    /* Fix for lprfsw: 2174 */
    /* check start time field, if it is zero, assign current time stamp */
    if(u32StartTime == 0)
    {
        // get time
        u32StartTime = u32ZCL_GetUTCTime();
    }


    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // initialize custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;

    // check number of events - zero is special case that means send back all events.
    if((u8NumberOfEvents == 0) || (u8NumberOfEvents > psPriceCustomDataStructure->u8NumberOfRecordEntries))
    {
        u8NumberOfEvents = psPriceCustomDataStructure->u8NumberOfRecordEntries;
    }

    // get head of alloc list - this is always the current price
    psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lPriceAllocList );

    // loop round event list until u8NumberOfEvents satisfied or list fully searched
    // psPublishPriceRecord == NULL will fall through - that is OK
    u8NumberOfEventsSent=0;
    while ((u8NumberOfEventsSent != u8NumberOfEvents) && (psPublishPriceRecord!=NULL))
    {
        /* check price start time, if it is zero get the time when event is added to list */
        if(psPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime == 0)
        {
            u32PriceEventStartTime = psPriceCustomDataStructure->u32ArrivalTimeOfStartNowEntry;
        }
        else
        {
            u32PriceEventStartTime = psPublishPriceRecord->sPublishPriceCmdPayload.u32StartTime;
        }

        /* Fix for lprfsw: 2172 */
        if(u32PriceEventStartTime +
                        (psPublishPriceRecord->sPublishPriceCmdPayload.u16DurationInMinutes * 60)    > u32StartTime)
        {
            u8Status = eSE_GeneratePublishPriceMessage(pZPSevent, psEndPointDefinition, psClusterInstance, &(psPublishPriceRecord->sPublishPriceCmdPayload));
            if (u8Status != E_ZCL_SUCCESS)
            {
                // Arrive here when time not synced on server.  Reply with default response and release mutex.
                eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SOFTWARE_FAILURE);
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return;
            }
            u8NumberOfEventsSent++;
        }
        psPublishPriceRecord = (tsSE_PricePublishPriceRecord *)psDLISTgetNext((DNODE *)psPublishPriceRecord);
    }

    // Not found any price messages to send so send a default response
    if(u8NumberOfEventsSent==0)
    {
        // reply with a default message
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


}

#ifdef BLOCK_CHARGING
/****************************************************************************
 **
 ** NAME:       vSE_HandleGetBlockPeriods
 **
 ** DESCRIPTION:
 ** Handles Price CLuster custom commands
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

PUBLIC  void vSE_HandleGetBlockPeriods(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    uint8 u8TransactionSequenceNumber;
    uint8 u8NumberOfEvents;
    uint32 u32StartTime, u32BlockPeriodStartTime;
    uint8 u8Status;
    uint8 u8NumberOfEventsSent;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord;
    //bool_t bIsAllEvents;

    eSE_GetBlockPeriodsReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &u32StartTime,
                    &u8NumberOfEvents);

    /* check start time field, if it is zero, assign current time stamp */
    if(u32StartTime == 0)
    {
        // get time
        u32StartTime = u32ZCL_GetUTCTime();
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;

    // check number of events - zero is special case that means send back all events.
    //bIsAllEvents = FALSE;
    if((u8NumberOfEvents == 0) || (u8NumberOfEvents > psPriceCustomDataStructure->u8NumberOfBlockRecordEntries))
    {
        u8NumberOfEvents = psPriceCustomDataStructure->u8NumberOfBlockRecordEntries;
        //bIsAllEvents = TRUE;
    }

    // get head of alloc list - this is always the current block period
    psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lBlockPeriodAllocList );

    // loop round event list until u8NumberOfEvents satisfied or list fully searched
    // psPublishBlockPeriodRecord == NULL will fall through - that is OK
    u8NumberOfEventsSent=0;
    while ((u8NumberOfEventsSent != u8NumberOfEvents) && (psPublishBlockPeriodRecord!=NULL))
    {
        /* check price start time, if it is zero get the time when event is added to list */
        if(psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime == 0)
        {
            u32BlockPeriodStartTime = psPriceCustomDataStructure->u32BlockPeriodArrivalTimeOfStartNowEntry;
        }
        else
        {
            u32BlockPeriodStartTime = psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime;
        }

        if(u32BlockPeriodStartTime +
                    (psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodDurationInMins * 60)    >= u32StartTime)
        /*if(psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload.u32BlockPeriodStartTime >= u32StartTime)*/
        {
            u8Status = eSE_GeneratePublishBlockPeriodMessage(pZPSevent, psEndPointDefinition,
                            psClusterInstance, &(psPublishBlockPeriodRecord->sPublishBlockPeriodCmdPayload));
            if (u8Status != E_ZCL_SUCCESS)
            {
                // Arrive here when time not synced on server.  Reply with default response and release mutex.
                eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SOFTWARE_FAILURE);
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return;
            }
            u8NumberOfEventsSent++;
        }
        psPublishBlockPeriodRecord = (tsSE_PricePublishBlockPeriodRecord *)psDLISTgetNext((DNODE *)psPublishBlockPeriodRecord);
    }

    // Not found any block period messages to send so send a default response
    if(u8NumberOfEventsSent==0)
    {
        // reply with a default message
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


}
#endif /* BLOCK_CHARGING */


#ifdef PRICE_CALORIFIC_VALUE

/****************************************************************************
 **
 ** NAME:       eSE_HandleGetCalorificValue
 **
 ** DESCRIPTION:
 ** Handles Price Get Calorific Value command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC  void vSE_HandleGetCalorificValue(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    uint8 u8Status;
    uint8 u8TransactionSequenceNumber;

    uint8   u8NumberOfEventsSent, u8NumberOfEvents;
    uint32 u32StartTime, u32CalorificValueStartTime;

    tsSE_PriceCalorificValueRecord *psPublishCalorificValueRecord;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;

    eSE_GetCalorificValueReceive(
            pZPSevent,
            &u8TransactionSequenceNumber,
            &u32StartTime,
            &u8NumberOfEvents);

    /* check start time field, if it is zero, assign current time stamp */
    if(u32StartTime == 0)
    {
        // get time
        u32StartTime = u32ZCL_GetUTCTime();
    }
    //get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;

    // check number of events - zero is special case that means send back all events.
     if((u8NumberOfEvents == 0) || (u8NumberOfEvents > psPriceCustomDataStructure->u8NumberOfCalorificValueEntries))
     {
         u8NumberOfEvents = psPriceCustomDataStructure->u8NumberOfCalorificValueEntries;
     }

    // get head of alloc list - this is always the current calorific value
    psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lCalorificValueAllocList );


    // loop round event list until u8NumberOfEvents satisfied or list fully searched
    // psPublishCalorificValueRecord == NULL will fall through - that is OK
    u8NumberOfEventsSent=0;
    while ((u8NumberOfEventsSent != u8NumberOfEvents) && (psPublishCalorificValueRecord!=NULL))
    {
        /* check calorific value start time, if it is zero get the time when event is added to list */
        if(psPublishCalorificValueRecord->sPublishCalorificValueCmdPayload.u32StartTime == 0)
        {
            u32CalorificValueStartTime = psPriceCustomDataStructure->u32CalorificArrivalTimeOfStartNowEntry;
        }
        else
        {
            u32CalorificValueStartTime = psPublishCalorificValueRecord->sPublishCalorificValueCmdPayload.u32StartTime;
        }

        if(u32CalorificValueStartTime   >= u32StartTime)
        {
            u8Status = eSE_GeneratePublishCalorificValueMessage(
                                          pZPSevent,
                                          &u8TransactionSequenceNumber)
                                          psClusterInstance,
                                          &psPublishCalorificValueRecord->sPublishCalorificValueCmdPayload);
            if (u8Status != E_ZCL_SUCCESS)
            {
                // Arrive here when time not synced on server.  Reply with default response and release mutex.
                eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SOFTWARE_FAILURE);
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return;
            }
            u8NumberOfEventsSent++;
        }
        psPublishCalorificValueRecord = (tsSE_PriceCalorificValueRecord *)psDLISTgetNext((DNODE *)psPublishCalorificValueRecord);
    }



    // Not found any block period messages to send so send a default response
    if(u8NumberOfEventsSent==0)
    {
        // reply with a default message
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


}

#endif


#ifdef PRICE_CONVERSION_FACTOR
/****************************************************************************
 **
 ** NAME:       eSE_HandleGetConversionFactor
 **
 ** DESCRIPTION:
 ** Handles Price Get Conversion Factor command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC  void vSE_HandleGetConversionFactor(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    tsPrice_Common *psPrice_Common;
    uint8 u8Status;
    uint8 u8TransactionSequenceNumber;
    uint8   u8SentRecords=0;
    uint8     u8NumberEvents;
    uint32  u32StartTime;

    tsSE_PriceConversionFactorRecord         *psPublishConversionFactorRecord,*psPrevPublishConversionFactorRecord=NULL;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;

    // get pricing option
    u8Status = eSE_GetConversionFactorReceive(pZPSevent, psEndPointDefinition, psClusterInstance, &u8TransactionSequenceNumber,&u32StartTime, &u8NumberEvents);

    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;
    // initialise pointer

    psPrice_Common = &psPriceCustomDataStructure->sPrice_Common;

    /*psPrice_Common->sPriceCallBackMessage.u32CurrentTime = u32StartTime;
    psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_GET_CONVERSION_FACTOR_RECEIVED;
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    */

    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // get head of alloc list - this is always the current price
    psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetHead( &psPriceCustomDataStructure->lConversionFactorAllocList );

    if(u8Status != E_ZCL_SUCCESS)
    {
        // reply with a default message
        eZCL_SendDefaultResponse(pZPSevent,E_ZCL_CMDS_SOFTWARE_FAILURE);
        // release mutex
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }


    // If there are no records in the L.L of the Conversion Factor send CMD NOT FOUND
    if(psPublishConversionFactorRecord == NULL )
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }


    // If start Time of Zero is passed, updated start time to present ZCL  Time.
    if(u32StartTime == 0 && u8NumberEvents != 0 )
        u32StartTime = u32ZCL_GetUTCTime();

    // We iterate over the L.L to find the last node with start time less than the passed start time, this will be the node
    // that is active at the mentioned start time.
    while(psPublishConversionFactorRecord != NULL && (psPublishConversionFactorRecord->sPublishConversionCmdPayload.u32StartTime < u32StartTime))
    {
        psPrevPublishConversionFactorRecord = psPublishConversionFactorRecord;
        psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetNext((DNODE *)psPublishConversionFactorRecord);
    }

    // If we have found atleast one record with start time less, is the active record
    if( psPrevPublishConversionFactorRecord != NULL )
        psPublishConversionFactorRecord = psPrevPublishConversionFactorRecord;

    // We send all the Requested Number of Conversion Factor Records to the Requesting node.
    while(psPublishConversionFactorRecord != NULL && ( (u8NumberEvents == 0 ) || (u8SentRecords < u8NumberEvents) ))
    {

        u8Status = eSE_GeneratePublishConversionFactorMessage(pZPSevent, psEndPointDefinition, psClusterInstance, &psPublishConversionFactorRecord->sPublishConversionCmdPayload);

        if(u8Status != E_ZCL_SUCCESS)
        {
               eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SOFTWARE_FAILURE);
               break;
        }

        u8SentRecords++;
        psPublishConversionFactorRecord = (tsSE_PriceConversionFactorRecord *)psDLISTgetNext((DNODE *)psPublishConversionFactorRecord);
    }

    if( u8SentRecords == 0)
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
    }

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}

#endif


/****************************************************************************
 **
 ** NAME:       vSE_HandlePublishPrice
 **
 ** DESCRIPTION:
 ** Handles Price CLuster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC  void vSE_HandlePublishPrice(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    tsPrice_Common *psPrice_Common;
    uint8 u8SEPriceReturn;
    uint8 u8TransactionSequenceNumber;

    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsSE_PricePublishPriceCmdPayload sPublishPriceCmdPayload;
    uint8 au8TempStringBuffer[SE_PRICE_SERVER_MAX_STRING_LENGTH + 1];

    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;
    // initialise pointer
    psPrice_Common = &psPriceCustomDataStructure->sPrice_Common;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // Setup temp string ptr
    sPublishPriceCmdPayload.sRateLabel.pu8Data = au8TempStringBuffer;
    sPublishPriceCmdPayload.sRateLabel.u8MaxLength = SE_PRICE_SERVER_MAX_STRING_LENGTH;
    // payload
    u8SEPriceReturn = eSE_PublishPriceReceive(pZPSevent, &sPublishPriceCmdPayload, &u8TransactionSequenceNumber, &(psPrice_Common->sPriceCallBackMessage.u32CurrentTime));

    vSE_PublishPriceResponse(pZPSevent, psEndPointDefinition, psClusterInstance, u8TransactionSequenceNumber, &sPublishPriceCmdPayload);

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    // fill in structure status
    psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = u8SEPriceReturn;

    if(u8SEPriceReturn != E_ZCL_SUCCESS)
    {
        // call user
        // Price callback
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_TABLE_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // Call user with the time in the message.  Gives the user a chance to synchronise time before
    // we attempt to add a message with a start time of now to the list.
    psPrice_Common->sPriceCallBackMessage.eEventType =  E_SE_PRICE_TIME_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

    // If the user hasn't synced time by now, we don't want to attemt to add anything to the lists
    if (!bZCL_GetTimeHasBeenSynchronised())
    {
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_TABLE_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

        // release EP and exit
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

#ifdef BLOCK_CHARGING
    /* Examine optional field u8NumberOfBlockThresholds in payload to find possible modes of operation :-
     * BLOCK or Combination Mode : sPublishPriceCmdPayload->u8NumberOfBlockThresholds != 0 and !=0xFF
     * */

    if (( 0 != sPublishPriceCmdPayload.u8NumberOfBlockThresholds) &&
        ( 0xff != sPublishPriceCmdPayload.u8NumberOfBlockThresholds) )
    {
        /* Error checking to make sure if the Server and Client have same number of attributes for block pricing,
         * If not, call the user informing the error and do not add the event to the event list.
         *  */
        if ( CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT < sPublishPriceCmdPayload.u8NumberOfBlockThresholds )
        {
            psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = E_SE_PRICE_NUMBER_OF_BLOCK_THRESHOLD_MISMATCH;
            psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_TABLE_ADD;
            psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

            // release EP and exit
            #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

            return;
        }
    }
#endif

    // add data - mutex protected
    u8SEPriceReturn = eSE_AddPriceEntryUsingPointer(psPriceCustomDataStructure, TRUE, &sPublishPriceCmdPayload);

    // If the event is not a duplicate, call user, this time informing of the list addition or failure to add
    if (u8SEPriceReturn != E_SE_PRICE_DUPLICATE)
    {
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = u8SEPriceReturn;
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_TABLE_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }

    #ifdef BLOCK_CHARGING
    if (( 0 != sPublishPriceCmdPayload.u8NumberOfBlockThresholds) &&
        ( 0xff != sPublishPriceCmdPayload.u8NumberOfBlockThresholds) )
    {
    /* Call user indicating to read the supplied pricing information in the CallBackFunction. */
    psPrice_Common->sPriceCallBackMessage.uMessage.sReadAttrInfo.psReceiveEventAddress= &psPrice_Common->sReceiveEventAddress;
    psPrice_Common->sPriceCallBackMessage.uMessage.sReadAttrInfo.u8NumberOfPriceTiersAndRegisterTiers = sPublishPriceCmdPayload.u8NumberOfPriceTiersAndRegisterTiers;
    psPrice_Common->sPriceCallBackMessage.uMessage.sReadAttrInfo.u8NumberOfBlockThresholds = sPublishPriceCmdPayload.u8NumberOfBlockThresholds;
    psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_READ_BLOCK_PRICING;
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }
    #endif

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}

/****************************************************************************
 **
 ** NAME:       eSE_GeneratePublishPriceMessage
 **
 ** DESCRIPTION:
 ** Handles Price CLuster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  teSE_PriceStatus eSE_GeneratePublishPriceMessage(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsSE_PricePublishPriceCmdPayload    *psPublishPriceCmdPayload)
{

    tsZCL_HeaderParams sZCL_HeaderParams;
    tsZCL_Address sZCL_Address;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);

    // issue publish price.  Use the TSN from the incoming message.
    return eSE_PricePublishPriceSend(
        // Message is a reply so call send with Src Ep set to the Dst Ep of the incoming message and
        // Dst Ep set to the Src Ep of the incoming message
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &sZCL_HeaderParams.u8TransactionSequenceNumber,
                    psPublishPriceCmdPayload);

}

#ifdef BLOCK_CHARGING
/****************************************************************************
 **
 ** NAME:       vSE_HandlePublishBlockPeriod
 **
 ** DESCRIPTION:
 ** Handles Price Cluster custom commands
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

PUBLIC  void vSE_HandlePublishBlockPeriod(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    tsPrice_Common *psPrice_Common;
    uint8 u8SEPriceReturn;
    uint8 u8TransactionSequenceNumber;

    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsSE_PricePublishBlockPeriodCmdPayload sPublishBlockPeriodCmdPayload;

    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;
    // initialise pointer
    psPrice_Common = &psPriceCustomDataStructure->sPrice_Common;


    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // payload
    u8SEPriceReturn = eSE_PublishBlockPeriodPriceReceive(pZPSevent, &sPublishBlockPeriodCmdPayload, &u8TransactionSequenceNumber, &(psPrice_Common->sPriceCallBackMessage.u32CurrentTime));

    vSE_PublishBlockPeriodResponse(pZPSevent, psEndPointDefinition, psClusterInstance, u8TransactionSequenceNumber, &sPublishBlockPeriodCmdPayload);

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    // fill in structure status
    psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = u8SEPriceReturn;

    if(u8SEPriceReturn != E_ZCL_SUCCESS)
    {
        // call user
        // Price callback
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_BLOCK_PERIOD_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
        // release EP
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // Call user with the time in the message.  Gives the user a chance to synchronise time before
    // we attempt to add a message with a start time of now to the list.
    psPrice_Common->sPriceCallBackMessage.eEventType =  E_SE_PRICE_TIME_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

    // If the user hasn't synced time by now, we don't want to attemt to add anything to the lists
    if (!bZCL_GetTimeHasBeenSynchronised())
    {
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_BLOCK_PERIOD_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

        // release EP and exit
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }


    /* Examine optional field u8NumberOfBlockThresholds in payload to find possible modes of operation :-
     * BLOCK or Combination Mode : sPublishPriceCmdPayload->u8NumberOfBlockThresholds != 0 and !=0xFF
     * */

    if ( ( 0 != sPublishBlockPeriodCmdPayload.b8NoOfTiersAndNoOfBlockThreshold ) &&
         ( 0xff != sPublishBlockPeriodCmdPayload.b8NoOfTiersAndNoOfBlockThreshold) )
    {
        /* Error checking to make sure the if the Server and Client have same number of attributes for block pricing,
         * If not, call the user informing the error and do not add the event to the event list.
         *  */
        if ( CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT < (sPublishBlockPeriodCmdPayload.b8NoOfTiersAndNoOfBlockThreshold & 0x0F) )
        {
            psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = E_SE_PRICE_NUMBER_OF_BLOCK_THRESHOLD_MISMATCH;
            psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_BLOCK_PERIOD_ADD;
            psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

            // release EP and exit
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            return;
        }
    }

    // add data - mutex protected
    u8SEPriceReturn = eSE_AddBlockPeriodEntryUsingPointer(psEndPointDefinition->u8EndPointNumber,
            FALSE, psPriceCustomDataStructure, TRUE, &sPublishBlockPeriodCmdPayload, NULL);

    // If the event is not a duplicate, call user, this time informing of the list addition or failure to add
    if (u8SEPriceReturn != E_SE_PRICE_DUPLICATE)
    {
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = u8SEPriceReturn;
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_BLOCK_PERIOD_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }

    /* Call user indicating to read the supplied pricing information in the CallBackFunction. */
    psPrice_Common->sPriceCallBackMessage.uMessage.sReadAttrInfo.psReceiveEventAddress= &psPrice_Common->sReceiveEventAddress;
    psPrice_Common->sPriceCallBackMessage.uMessage.sReadAttrInfo.u8NumberOfBlockThresholds = (sPublishBlockPeriodCmdPayload.b8NoOfTiersAndNoOfBlockThreshold & 0x0F);
    psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_READ_BLOCK_THRESHOLDS;
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}


/****************************************************************************
 **
 ** NAME:       eSE_GeneratePublishBlockPeriodMessage
 **
 ** DESCRIPTION:
 ** Handles Price CLuster custom commands
 **
 ** PARAMETERS:                               Name                                  Usage
 ** ZPS_tsAfEvent                              *pZPSevent                         Zigbee stack event structure
 ** tsZCL_EndPointDefinition                 *psEndPointDefinition              EP structure
 ** tsZCL_ClusterInstance                    *psClusterInstance                 Cluster structure
 ** tsSE_PricePublishBlockPeriodCmdPayload  *psPublishBlockPeriodCmdPayload Block Period Cmd Payload
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  teSE_PriceStatus eSE_GeneratePublishBlockPeriodMessage(
                    ZPS_tsAfEvent                           *pZPSevent,
                    tsZCL_EndPointDefinition                *psEndPointDefinition,
                    tsZCL_ClusterInstance                   *psClusterInstance,
                    tsSE_PricePublishBlockPeriodCmdPayload  *psPublishBlockPeriodCmdPayload)
{

    tsZCL_HeaderParams sZCL_HeaderParams;
    tsZCL_Address sZCL_Address;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);

    // issue publish block period.  Use the TSN from the incoming message.
    return eSE_PricePublishBlockPeriodSend(
        // Message is a reply so call send with Src Ep set to the Dst Ep of the incoming message and
        // Dst Ep set to the Src Ep of the incoming message
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &sZCL_HeaderParams.u8TransactionSequenceNumber,
                    psPublishBlockPeriodCmdPayload);
}

#endif



/****************************************************************************
 **
 ** NAME:       vSE_PublishPriceResponse
 **
 ** DESCRIPTION:
 ** Generate a default or pricke ack response to an incoming publish price message
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** tsSE_PricePublishPriceCmdPayload    *psPublishPriceCmdPayload Incoming price command
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  void vSE_PublishPriceResponse(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8TransactionSequenceNumber,
                    tsSE_PricePublishPriceCmdPayload    *psPublishPriceCmdPayload)
{

    tsZCL_HeaderParams sZCL_HeaderParams;
    tsZCL_Address sZCL_Address;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);


    if ((psPublishPriceCmdPayload->u8PriceControl & PRICE_ACK_REQUIRED_MASK) != 0 &&
        psPublishPriceCmdPayload->u8PriceControl != 0xff)  // If field not present, u8PriceControl set to 0xff - invalid.  Need to check for this: RT 5239 / lpsw1889

    {
        tsSE_PriceAckCmdPayload sAckPayload;
        sAckPayload.u32IssuerEventId = psPublishPriceCmdPayload->u32IssuerEventId;
        sAckPayload.u32ProviderId    = psPublishPriceCmdPayload->u32ProviderId;
        sAckPayload.u32PriceAckTime  = u32ZCL_GetUTCTime();
        sAckPayload.u8Control        = psPublishPriceCmdPayload->u8PriceControl;

        eSE_PriceAckCommandSend(
                    // Message is a reply so call send with Src Ep set to the Dst Ep of the incoming message and
        // Dst Ep set to the Src Ep of the incoming message
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &sZCL_HeaderParams.u8TransactionSequenceNumber,
                    &sAckPayload);
        return;
    }

    // Generate success default response if no price ack and other appropriate conditions met

    /* If a default response is required by the sender and incoming message was sent unicast, send one */
    if((sZCL_HeaderParams.bDisableDefaultResponse == FALSE) &&
        (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_SHORT || pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_SHORT))
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
    }
}


#ifdef BLOCK_CHARGING
/****************************************************************************
 **
 ** NAME:       vSE_PublishBlockPeriodResponse
 **
 ** DESCRIPTION:
 ** Generate a default or pricke ack response to an incoming publish price message
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** tsSE_PricePublishBlockPeriodCmdPayload *spPublishBlockPeriodCmdPayload Incoming price command
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  void vSE_PublishBlockPeriodResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8TransactionSequenceNumber,
                    tsSE_PricePublishBlockPeriodCmdPayload *psPublishBlockPeriodCmdPayload)
{

    tsZCL_HeaderParams sZCL_HeaderParams;

    tsZCL_Address sZCL_Address;
    // set to 0 - M$VC cludge to get round the problem of enums being longs
    eFrameTypeSubfield    eFrameType=0;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);

    if ((psPublishBlockPeriodCmdPayload->b8BlockPeriodControl & PRICE_ACK_REQUIRED_MASK) != 0 &&
            psPublishBlockPeriodCmdPayload->b8BlockPeriodControl != 0xff)  // If field not present, u8PriceControl set to 0xff - invalid.  Need to check for this: RT 5239 / lpsw1889

    {
        tsSE_PriceAckCmdPayload sAckPayload;
        sAckPayload.u32IssuerEventId = psPublishBlockPeriodCmdPayload->u32IssuerEventId;
        sAckPayload.u32ProviderId    = psPublishBlockPeriodCmdPayload->u32ProviderId;
        sAckPayload.u32PriceAckTime  = u32ZCL_GetUTCTime();
        sAckPayload.u8Control        = psPublishBlockPeriodCmdPayload->b8BlockPeriodControl;

        eSE_PriceAckCommandSend(
                    // Message is a reply so call send with Src Ep set to the Dst Ep of the incoming message and
        // Dst Ep set to the Src Ep of the incoming message
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &u8TransactionSequenceNumber,
                    &sAckPayload);
        return;
    }

    // Generate success default response if no price ack and other appropriate conditions met

    /* If a default response is required by the sender and incoming message was sent unicast, send one */
    if((bDisableDefaultResponse == FALSE) &&
        (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_SHORT || pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_SHORT))
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
    }

}
#endif /* BLOCK_CHARGING */



/****************************************************************************
 **
 ** NAME:       vSE_HandlePriceAck
 **
 ** DESCRIPTION:
 ** Handles Price Acknowledgement custom command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC  void vSE_HandlePriceAck(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    tsPrice_Common *psPrice_Common;
    uint8 u8TransactionSequenceNumber;
    tsSE_PriceAckCmdPayload  sPriceAck;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;

    // get pricing option
     eSE_PriceAckReceive(pZPSevent, &u8TransactionSequenceNumber, &sPriceAck);

    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;
    // initialise pointer
    psPrice_Common = &psPriceCustomDataStructure->sPrice_Common;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    // Price callback
    psPrice_Common->sPriceCallBackMessage.u32CurrentTime = 0; // Time not relevant for server callback
    psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_ACK_RECEIVED;

    psPrice_Common->sPriceCallBackMessage.uMessage.psAckCmdPayload = &sPriceAck;
    // call user
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}

#ifdef PRICE_CONVERSION_FACTOR

/****************************************************************************
 **
 ** NAME:       eSE_GeneratePublishConversionFactorMessage
 **
 ** DESCRIPTION:
 ** Handles Price CLuster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  teSE_PriceStatus eSE_GeneratePublishConversionFactorMessage(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsSE_PricePublishConversionCmdPayload    *psPublishConversionCmdPayload)
{

    tsZCL_HeaderParams sZCL_HeaderParams;
    tsZCL_Address sZCL_Address;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);

    // issue publish price.  Use the TSN from the incoming message.
    return  eSE_PricePublishConversionFactorSend(
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &sZCL_HeaderParams.u8TransactionSequenceNumber,
                    psPublishConversionCmdPayload);

}


/****************************************************************************
 **
 ** NAME:       vSE_HandlePublishConversionFactor
 **
 ** DESCRIPTION:
 ** Handles Price Cluster custom commands
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

PUBLIC  void vSE_HandlePublishConversionFactor(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    tsPrice_Common *psPrice_Common;
    teSE_PriceStatus u8SEPriceReturn;
    uint8 u8TransactionSequenceNumber;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;

    tsSE_PricePublishConversionCmdPayload sPublishConversionCmdPayload;

    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;
    // initialise pointer
    psPrice_Common = &psPriceCustomDataStructure->sPrice_Common;


    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    u8SEPriceReturn = eSE_PublishConversionFactorReceive(pZPSevent, &sPublishConversionCmdPayload, &u8TransactionSequenceNumber, &(psPrice_Common->sPriceCallBackMessage.u32CurrentTime));

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = u8SEPriceReturn;

    if(u8SEPriceReturn != E_ZCL_SUCCESS)
    {

        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_CONVERSION_FACTOR_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return;
    }

    // Call user with the time in the message.  Gives the user a chance to synchronise time before
    // we attempt to add a message with a start time of now to the list.

    /*psPrice_Common->sPriceCallBackMessage.eEventType =  E_SE_CONVERSION_FACTOR_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    */

    // If the user hasn't synced time by now, we don't want to attemt to add anything to the lists
    if (!bZCL_GetTimeHasBeenSynchronised())
    {
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_CONVERSION_FACTOR_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
        // release EP and exit
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // add data - mutex protected
    u8SEPriceReturn = eSE_AddConversionFactorUsingPointer(psEndPointDefinition->u8EndPointNumber,FALSE, psPriceCustomDataStructure, TRUE, &sPublishConversionCmdPayload);
    // If the event is not a duplicate, call user, this time informing of the list addition or failure to add

    if (u8SEPriceReturn != E_SE_PRICE_DUPLICATE)
    {
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = u8SEPriceReturn;
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_CONVERSION_FACTOR_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }


    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}

#endif


#ifdef PRICE_CALORIFIC_VALUE

/****************************************************************************
 **
 ** NAME:       eSE_GeneratePublishCalorificValueMessage
 **
 ** DESCRIPTION:
 ** Handles Price CLuster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  teSE_PriceStatus eSE_GeneratePublishCalorificValueMessage(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsSE_PricePublishCalorificValueCmdPayload    *psPublishCalorificCmdPayload)
{

    tsZCL_HeaderParams sZCL_HeaderParams;
    tsZCL_Address sZCL_Address;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);

    // issue publish price.  Use the TSN from the incoming message.
    return  eSE_PricePublishCalorificValueSend(
        // Message is a reply so call send with Src Ep set to the Dst Ep of the incoming message and
        // Dst Ep set to the Src Ep of the incoming message
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &sZCL_HeaderParams.u8TransactionSequenceNumber,
                    psPublishCalorificCmdPayload);

}

/****************************************************************************
 **
 ** NAME:       vSE_HandlePublishCalorificValue
 **
 ** DESCRIPTION:
 ** Handles Price Cluster custom commands
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

PUBLIC  void vSE_HandlePublishCalorificValue(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    tsPrice_Common *psPrice_Common;
    uint8 u8SEPriceReturn;
    uint8 u8TransactionSequenceNumber;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;

    tsSE_PricePublishCalorificValueCmdPayload sPublishCalorificValueCmdPayload;

    // initialise custom data pointer
    psPriceCustomDataStructure = (tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr;
    // initialise pointer
    psPrice_Common = &psPriceCustomDataStructure->sPrice_Common;


    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    u8SEPriceReturn = eSE_PublishCalorificValueReceive(pZPSevent, &sPublishCalorificValueCmdPayload, &u8TransactionSequenceNumber, &(psPrice_Common->sPriceCallBackMessage.u32CurrentTime));

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psPrice_Common->sPriceCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = u8SEPriceReturn;

    if(u8SEPriceReturn != E_ZCL_SUCCESS)
    {

        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_CALORIFIC_VALUE_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return;
    }

    // Call user with the time in the message.  Gives the user a chance to synchronise time before
    // we attempt to add a message with a start time of now to the list.
    /*
    psPrice_Common->sPriceCallBackMessage.eEventType =  E_SE_CALORIFIC_VALUE_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    */

    // If the user hasn't synced time by now, we don't want to attemt to add anything to the lists
    if (!bZCL_GetTimeHasBeenSynchronised())
    {
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = E_ZCL_ERR_TIME_NOT_SYNCHRONISED;
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_CALORIFIC_VALUE_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);

        // release EP and exit
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return;
    }

    // add data - mutex protected
    u8SEPriceReturn = eSE_AddCalorificValueEntryUsingPointer(psEndPointDefinition->u8EndPointNumber,FALSE, psPriceCustomDataStructure, TRUE, &sPublishCalorificValueCmdPayload);

    // If the event is not a duplicate, call user, this time informing of the list addition or failure to add

    if (u8SEPriceReturn != E_SE_PRICE_DUPLICATE)
    {
        psPrice_Common->sPriceCallBackMessage.uMessage.sPriceTableCommand.ePriceStatus = u8SEPriceReturn;
        psPrice_Common->sPriceCallBackMessage.eEventType = E_SE_PRICE_CALORIFIC_VALUE_ADD;
        psEndPointDefinition->pCallBackFunctions(&psPrice_Common->sPriceCustomCallBackEvent);
    }


    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}

#endif


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

