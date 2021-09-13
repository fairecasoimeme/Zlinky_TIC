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
 * COMPONENT:          price_internal.h
 *
 * DESCRIPTION:        The internal API for the Price Cluster
 *
 *****************************************************************************/

#ifndef  PRICE_INTERNAL_H_INCLUDED
#define  PRICE_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "Price.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teSE_PriceStatus eSE_PublishPriceReceive(
                    ZPS_tsAfEvent              *pZPSevent,
                    tsSE_PricePublishPriceCmdPayload   *psPublishPriceCmdPayload,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint32                     *pu32CurrentTimeInMessage);

PUBLIC teSE_PriceStatus eSE_GetScheduledPricesReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint32                      *pu32StartTime,
                    uint8                       *pu8NumberOfEvents);

PUBLIC teSE_PriceStatus eSE_GetBlockPeriodsReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint32                      *pu32StartTime,
                    uint8                       *pu8NumberOfEvents);

PUBLIC teSE_PriceStatus eSE_GetCurrentPriceReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    teSE_PriceCommandOptions    *pePriceCommandOptions);

PUBLIC void vSE_HandleGetCurrentPrice(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_HandleGetSheduledPrices(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_HandleGetBlockPeriods(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_HandlePublishPrice(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC teSE_PriceStatus eSE_AddPriceEntryUsingPointer(
                    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                    bool_t                      bOverwritePrevious,
                    tsSE_PricePublishPriceCmdPayload   *psPublishPriceCmdPayload);

#ifdef BLOCK_CHARGING
PUBLIC teSE_PriceStatus eSE_AddBlockPeriodEntryUsingPointer(
            uint8                                     u8SourceEndPointId,
			bool_t                                    bIsServer,
            tsSE_PriceCustomDataStructure             *psPriceCustomDataStructure,
            bool_t                                  bOverwritePrevious,
            tsSE_PricePublishBlockPeriodCmdPayload  *psPublishBlockPeriodCmdPayload,
            tsSE_BlockThresholds                    *psBlockThresholds);
#endif /* BLOCK_CHARGING */

PUBLIC teSE_PriceStatus eSE_FindPriceCluster(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    tsZCL_EndPointDefinition    **ppsEndPointDefinition,
                    tsZCL_ClusterInstance       **ppsClusterInstance,
                    tsSE_PriceCustomDataStructure  **ppsPriceCustomDataStructure);

PUBLIC void vSE_PriceTimerClickCallback(
                    tsZCL_CallBackEvent        *psCallBackEvent);

PUBLIC teSE_PriceStatus eSE_GetNumberOfFreePriceTableEntries(
                    tsZCL_EndPointDefinition   *psEndPointDefinition,
                    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                    uint8                      *pu8NumberOfEntries);

PUBLIC teSE_PriceStatus eSE_GetNumberOfFreeBlockPeriodTableEntries(
                            tsZCL_EndPointDefinition *psEndPointDefinition,
                            tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                            uint8 *pu8NumberOfEntries);

PUBLIC teSE_PriceStatus eSE_PricePublishPriceSend(
                    uint8                               u8SourceEndPointId,
                    uint8                               u8DestinationEndPointId,
                    tsZCL_Address                       *psDestinationAddress,
                    uint8                               *pu8TransactionSequenceNumber,
                    tsSE_PricePublishPriceCmdPayload    *psPublishPriceCmdPayload);

PUBLIC teZCL_Status eSE_PriceCommandHandler(
                    ZPS_tsAfEvent               *pZPSEvent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC teSE_PriceStatus eSE_PriceRegisterPriceTimeServer(void);

PUBLIC teSE_PriceStatus eSE_PriceAckCommandSend(
                    uint8                      u8SourceEndPointId,
                    uint8                      u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    tsSE_PriceAckCmdPayload    *psPriceAckPayload);

PUBLIC void vSE_HandlePriceAck(
                    ZPS_tsAfEvent                 *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC teSE_PriceStatus eSE_PriceAckReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsSE_PriceAckCmdPayload     *psPriceAckPayload);


PUBLIC teSE_PriceStatus eSE_PricePublishBlockPeriodSend(
                    uint8                                       u8SourceEndPointId,
                    uint8                                       u8DestinationEndPointId,
                    tsZCL_Address                               *psDestinationAddress,
                    uint8                                       *pu8TransactionSequenceNumber,
                    tsSE_PricePublishBlockPeriodCmdPayload        *psPublishBlockPeriodCmdPayload);

PUBLIC teSE_PriceStatus eSE_PublishBlockPeriodPriceReceive(
                            ZPS_tsAfEvent              *pZPSevent,
                            tsSE_PricePublishBlockPeriodCmdPayload   *psPublishBlockPeriodCmdPayload,
                            uint8                      *pu8TransactionSequenceNumber,
                            uint32                     *pu32CurrentTimeInMessage);

PUBLIC void vSE_HandlePublishBlockPeriod(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);


#ifdef PRICE_CONVERSION_FACTOR

PUBLIC teSE_PriceStatus eSE_PublishConversionFactorReceive(
                            ZPS_tsAfEvent              *pZPSevent,
                            tsSE_PricePublishConversionCmdPayload   *psPublishConversionFactorCmdPayload,
                            uint8                      *pu8TransactionSequenceNumber,
                            uint32                     *pu32CurrentTimeInMessage);

PUBLIC teSE_PriceStatus eSE_PricePublishConversionFactorSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    tsSE_PricePublishConversionCmdPayload   *psPublishConversionFactorCmdPayload);

PUBLIC teSE_PriceStatus eSE_GetConversionFactorReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint32                        *pu32StartTime,
                    uint8                        *pu8NumberOfEvents);


PUBLIC void vSE_HandleGetConversionFactor(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_HandlePublishConversionFactor(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC teSE_PriceStatus eSE_AddConversionFactorUsingPointer(
            uint8                                   u8SourceEndPointId,
			bool_t                                    bIsServer,
            tsSE_PriceCustomDataStructure           *psPriceCustomDataStructure,
            bool_t                                  bOverwritePrevious,
            tsSE_PricePublishConversionCmdPayload   *psPublishConversionCmdPayload);

PUBLIC teSE_PriceStatus eSE_GetNumberOfFreeConversionFactorTableEntries(
         tsZCL_EndPointDefinition       *psEndPointDefinition,
         tsSE_PriceCustomDataStructure  *psPriceCustomDataStructure,
         uint8                          *pu8NumberOfEntries);
#endif

#ifdef PRICE_CALORIFIC_VALUE

PUBLIC teSE_PriceStatus eSE_GetCalorificValueReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint32                        *pu32StartTime,
                    uint8                        *pu8NumberOfEvents);


PUBLIC teSE_PriceStatus eSE_PricePublishCalorificValueSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    tsSE_PricePublishCalorificValueCmdPayload   *psPublishCalorificValueCmdPayload);



PUBLIC void vSE_HandleGetCalorificValue(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC void vSE_HandlePublishCalorificValue(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);


PUBLIC teSE_PriceStatus eSE_PublishCalorificValueReceive(
                            ZPS_tsAfEvent              *pZPSevent,
                            tsSE_PricePublishCalorificValueCmdPayload   *psPublishCalorificValueCmdPayload,
                            uint8                      *pu8TransactionSequenceNumber,
                            uint32                     *pu32CurrentTimeInMessage);

PUBLIC teSE_PriceStatus eSE_AddCalorificValueEntryUsingPointer(
         uint8                                      u8SourceEndPointId,
		 bool_t                                       bIsServer,
         tsSE_PriceCustomDataStructure              *psPriceCustomDataStructure,
         bool_t                                     bOverwritePrevious,
         tsSE_PricePublishCalorificValueCmdPayload  *psPublishCalorificValueCmdPayload);

PUBLIC teSE_PriceStatus eSE_GetNumberOfFreeCalorificValueTableEntries(
         tsZCL_EndPointDefinition       *psEndPointDefinition,
         tsSE_PriceCustomDataStructure  *psPriceCustomDataStructure,
         uint8                          *pu8NumberOfEntries);

#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* PRICE_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
