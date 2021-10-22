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
 * COMPONENT:          Price.c
 *
 * DESCRIPTION:        Price Cluster Definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>

#include "zcl.h"
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

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/* Define the attributes in the simple metering cluster */
const tsZCL_AttributeDefinition asCLD_PriceClusterAttributeDefinitions[] = {

        /* Tier Price Label Set (D.4.2.2.1) */
#if (CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT != 0)
#if((CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT <= 6))
        {E_CLD_P_ATTR_TIER_1_PRICE_LABEL,   (E_ZCL_AF_RD|E_ZCL_AF_WR),     E_ZCL_OSTRING,    (uint32)(&((tsCLD_Price*)(0))->asTierPriceLabel[0]), CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT -1 },
#else
        {E_CLD_P_ATTR_TIER_1_PRICE_LABEL,   (E_ZCL_AF_RD|E_ZCL_AF_WR),     E_ZCL_OSTRING,    (uint32)(&((tsCLD_Price*)(0))->asTierPriceLabel[0]), 6 -1 },
        {E_CLD_P_ATTR_TIER_7_PRICE_LABEL,   (E_ZCL_AF_RD),     E_ZCL_OSTRING,    (uint32)(&((tsCLD_Price*)(0))->asTierPriceLabel[6]), CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT - 6 -1 },
#endif
#endif

    /* Block Threshold Set (D.4.2.2.2) */
#if (CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT != 0)
    {E_CLD_P_ATTR_BLOCK1_THRESHOLD,   (E_ZCL_AF_RD),     E_ZCL_UINT48,    (uint32)(&((tsCLD_Price*)(0))->au48BlockThreshold[0]), CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT - 1},
#endif

    /* Block Period Attribute Set (D.4.2.2.3) */
#ifdef CLD_P_ATTR_START_OF_BLOCK_PERIOD
    {E_CLD_P_ATTR_START_OF_BLOCK_PERIOD,   (E_ZCL_AF_RD),     E_ZCL_UTCT,    (uint32)(&((tsCLD_Price*)(0))->utctStartOfBlockPeriod), 0},
#endif

#ifdef CLD_P_ATTR_BLOCK_PERIOD_DURATION
    {E_CLD_P_ATTR_BLOCK_PERIOD_DURATION,   (E_ZCL_AF_RD),     E_ZCL_UINT24,    (uint32)(&((tsCLD_Price*)(0))->u24BlockPeriodDuration), 0},
#endif

#ifdef CLD_P_ATTR_THRESHOLD_MULTIPLIER
    {E_CLD_P_ATTR_THRESHOLD_MULTIPLIER,   (E_ZCL_AF_RD),     E_ZCL_UINT24,    (uint32)(&((tsCLD_Price*)(0))->u24ThresholdMultiplier), 0},
#endif

#ifdef CLD_P_ATTR_THRESHOLD_DIVISOR
    {E_CLD_P_ATTR_THRESHOLD_DIVISOR,   (E_ZCL_AF_RD),     E_ZCL_UINT24,    (uint32)(&((tsCLD_Price*)(0))->u24ThresholdDivisor), 0},
#endif


    /* Commodity Set (D.4.2.2.4) */
#ifdef CLD_P_ATTR_COMMODITY_TYPE
    {E_CLD_P_ATTR_COMMODITY_TYPE,   (E_ZCL_AF_RD),     E_ZCL_ENUM8,    (uint32)(&((tsCLD_Price*)(0))->e8CommodityType), 0},
#endif

#ifdef CLD_P_ATTR_STANDING_CHARGE
    {E_CLD_P_ATTR_STANDING_CHARGE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->u32StandingCharge), 0},
#endif

#ifdef CLD_P_ATTR_CONVERSION_FACTOR
    {E_CLD_P_ATTR_CONVERSION_FACTOR, (E_ZCL_AF_RD) , E_ZCL_UINT32,  (uint32)(&((tsCLD_Price*)(0))->u32ConversionFactor), 0},
#endif

#ifdef CLD_P_ATTR_CONVERSION_FACTOR_TRAILING_DIGIT
    {E_CLD_P_ATTR_CONVERSION_FACTOR_TRAILING_DIGIT, (E_ZCL_AF_RD), E_ZCL_BMAP8, (uint32)(&((tsCLD_Price*)(0))->b8ConversionFactorTrailingDigit), 0},
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE
    {E_CLD_P_ATTR_CALORIFIC_VALUE, (E_ZCL_AF_RD) , E_ZCL_UINT32 , (uint32)(&((tsCLD_Price*)(0))->u32CalorificValue), 0},
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE_UNIT
    {E_CLD_P_ATTR_CALORIFIC_VALUE_UNIT, (E_ZCL_AF_RD) , E_ZCL_ENUM8,  (uint32)(&((tsCLD_Price*)(0))->e8CalorificValueUnit), 0},
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE_TRAILING_DIGIT
    {E_CLD_P_ATTR_CALORIFIC_VALUE_TRAILING_DIGIT, (E_ZCL_AF_RD), E_ZCL_BMAP8, (uint32)(&((tsCLD_Price*)(0))->b8CalorificValueTrailingDigit), 0},
#endif

    /* Block Price Information Set (D.4.2.2.5) */
#if (CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT != 0)
    {E_CLD_P_ATTR_NOTIER_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32NoTierBlockPrice[0]), CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 0)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER1_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier1BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 1)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER2_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier2BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 2)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER3_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier3BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 3)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER4_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier4BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 4)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER5_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier5BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 5)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER6_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier6BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 6)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER7_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier7BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 7)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER8_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier8BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 8)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER9_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier9BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 9)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER10_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier10BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 10)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER11_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier11BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 11)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER12_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier12BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 12)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER13_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier13BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 13)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER14_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier14BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 14)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    {E_CLD_P_ATTR_TIER15_BLOCK1_PRICE,   (E_ZCL_AF_RD),     E_ZCL_UINT32,    (uint32)(&((tsCLD_Price*)(0))->au32Tier15BlockPrice[0]), CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE -1},
#endif

// Billing period - new for RIB
#ifdef CLD_P_ATTR_START_OF_BILLING_PERIOD
    {E_CLD_P_ATTR_START_OF_BILLING_PERIOD,   (E_ZCL_AF_RD),     E_ZCL_UTCT,    (uint32)(&((tsCLD_Price*)(0))->utctStartOfBillingPeriod), 0},
#endif

#ifdef CLD_P_ATTR_BILLING_PERIOD_DURATION
    {E_CLD_P_ATTR_BILLING_PERIOD_DURATION,   (E_ZCL_AF_RD),     E_ZCL_UINT24,    (uint32)(&((tsCLD_Price*)(0))->u24BillingPeriodDuration), 0},
#endif
#ifdef CLD_P_CLIENT_ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES
    {E_CLD_P_CLIENT_ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES,   (E_ZCL_AF_RD|E_ZCL_AF_WR | E_ZCL_AF_CA),     E_ZCL_UINT8,    (uint32)(&((tsCLD_Price*)(0))->u8ClientIncreaseRandomize), 0},
#endif
#ifdef CLD_P_CLIENT_ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES
    {E_CLD_P_CLIENT_ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES,   (E_ZCL_AF_RD|E_ZCL_AF_WR | E_ZCL_AF_CA),     E_ZCL_UINT8,    (uint32)(&((tsCLD_Price*)(0))->u8ClientDecreaseRandomize), 0},
#endif
#ifdef CLD_P_CLIENT_ATTR_COMMODITY_TYPE
    {E_CLD_P_CLIENT_ATTR_COMMODITY_TYPE,  (E_ZCL_AF_RD | E_ZCL_AF_CA),     E_ZCL_ENUM8,    (uint32)(&((tsCLD_Price*)(0))->e8ClientCommodityType), 0},
#endif

};


#if(CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT != 0)
#define CLD_P_ATTR_NUM_OF_PRICE         CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT - 1
#elif (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0)
#define CLD_P_ATTR_NUM_OF_PRICE         CLD_P_ATTR_NUM_OF_TIERS_PRICE*(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE - 1)
#else
#define CLD_P_ATTR_NUM_OF_PRICE         0
#endif

#if (CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT != 0)
#if((CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT <= 6))
#define CLD_P_ATTR_NUM_OF_PRICE_LABEL            CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT -1
#else
#define CLD_P_ATTR_NUM_OF_PRICE_LABEL            CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT -2
#endif
#else
#define CLD_P_ATTR_NUM_OF_PRICE_LABEL            0
#endif

#if (CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT != 0)
#define CLD_P_ATTR_NUM_OF_BLOCK_THRESHOLD         CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT -1
#else
#define CLD_P_ATTR_NUM_OF_BLOCK_THRESHOLD         0
#endif

#define PRICE_NUM_OF_ATTRIBUTES      sizeof(asCLD_PriceClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition) + \
                                     CLD_P_ATTR_NUM_OF_PRICE +\
                                     CLD_P_ATTR_NUM_OF_PRICE_LABEL+\
                                     CLD_P_ATTR_NUM_OF_BLOCK_THRESHOLD


#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
const tsZCL_CommandDefinition asCLD_PriceClusterCommandDefinitions[] = {
    {E_SE_GET_CURRENT_PRICE,                                E_ZCL_CF_RX},     /* Mandatory */
    {E_SE_GET_SCHEDULED_PRICES,                             E_ZCL_CF_RX},     /* Mandatory */
    {E_SE_PRICE_ACK,                                        E_ZCL_CF_RX},     /* Mandatory */   
    {E_SE_GET_BLOCK_PERIOD,                                 E_ZCL_CF_RX},     /* Mandatory */
    {E_SE_GET_CONVERSION_FACTOR,                            E_ZCL_CF_RX},     /* Mandatory */
    {E_SE_GET_CALORIFIC_VALUE,                              E_ZCL_CF_RX},     /* Mandatory */
     
    {E_SE_PUBLISH_PRICE,                                    E_ZCL_CF_TX},     /* Mandatory */ 
    {E_SE_PUBLISH_BLOCK_PERIOD,                             E_ZCL_CF_TX},     /* Mandatory */
    {E_SE_PUBLISH_CALORIFIC_VALUE,                          E_ZCL_CF_TX},     /* Mandatory */
    {E_SE_PUBLISH_CONVERSION_FACTOR,                        E_ZCL_CF_TX}      /* Mandatory */
    
};
#endif
tsZCL_ClusterDefinition sCLD_Price = {
        SE_CLUSTER_ID_PRICE,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        PRICE_NUM_OF_ATTRIBUTES,
        (tsZCL_AttributeDefinition*)asCLD_PriceClusterAttributeDefinitions,
        NULL
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
        ,
        (sizeof(asCLD_PriceClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
        (tsZCL_CommandDefinition*)asCLD_PriceClusterCommandDefinitions
#endif        
};

uint8   au8PriceClusterAttributeControlBits[PRICE_NUM_OF_ATTRIBUTES];
/****************************************************************************
 **
 ** NAME:       eSE_PriceCreate
 **
 ** DESCRIPTION:
 ** Creates Price device
 **
 ** PARAMETERS:                 Name                        Usage
 ** bool_t                       bIsServer                  Server/Client
 ** uint8                        u8NumberOfRecordEntries    number of messages
 ** uint8                        *pu8AttributeControlBits   attribute control bits
 ** uint8                       *pau8RateLabel              rate label storage
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster structure
 ** tsZCL_ClusterDefinition     *psClusterDefinition        Cluster definition
 ** tsZCL_AttributeStatus       *psAttributeStatus          Attribute state data
 ** tsSE_PriceCustomDataStructure *psCustomDataStructure    Custom data structure
 ** tsSE_PricePublishPriceRecord *psPublishPriceRecord      Publish data
 ** void                        *pvEndPointSharedStructPtr  Cluster Data
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eSE_PriceCreate(
                bool_t                              bIsServer,
                uint8                               u8NumberOfRecordEntries,
                uint8                              *pu8AttributeControlBits,
                uint8                              *pau8RateLabel,
                tsZCL_ClusterInstance              *psClusterInstance,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                tsSE_PriceCustomDataStructure      *psCustomDataStructure,
                tsSE_PricePublishPriceRecord       *psPublishPriceRecord,
                void                               *pvEndPointSharedStructPtr
#ifdef BLOCK_CHARGING
                ,tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord,
                uint8                               u8NumberOfBlockPeriodRecordEntries
#endif
#ifdef PRICE_CONVERSION_FACTOR
                ,tsSE_PriceConversionFactorRecord   *psConversionFactorRecord,
                uint8                                u8NumberOfConversionFactorRecordEntries
#endif
#ifdef PRICE_CALORIFIC_VALUE
                ,tsSE_PriceCalorificValueRecord        *psCalorificValueRecord,
                uint8                                u8NumberOfCalorificValueRecordEntries
#endif
                )
{
    tsPrice_Common *psPrice_Common;
    int i;

    #ifdef STRICT_PARAM_CHECK 
        if((psClusterInstance==NULL)            ||
           (psClusterDefinition==NULL)          ||
           (pvEndPointSharedStructPtr==NULL)    ||
           (psCustomDataStructure==NULL)
    #ifdef BLOCK_CHARGING
           ||
           (psPublishPriceRecord==NULL)            ||
           (psPublishBlockPeriodRecord==NULL)
    #endif /* BLOCK_CHARGING */
    #ifdef PRICE_CONVERSION_FACTOR
          || (psConversionFactorRecord == NULL )
    #endif
    #ifdef PRICE_CALORIFIC_VALUE
          || (psCalorificValueRecord == NULL )
    #endif
        )
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    
    if(bIsServer)
    {
        if(SE_PRICE_NUMBER_OF_SERVER_PRICE_RECORD_ENTRIES < u8NumberOfRecordEntries)
        {
            return(E_ZCL_ERR_PARAMETER_RANGE);
        }
    }
    else
    {
        if(SE_PRICE_NUMBER_OF_CLIENT_PRICE_RECORD_ENTRIES < u8NumberOfRecordEntries)
        {
            return(E_ZCL_ERR_PARAMETER_RANGE);
        }
    }
#ifdef BLOCK_CHARGING
    if(bIsServer)
    {
        if(SE_PRICE_NUMBER_OF_SERVER_BLOCK_PERIOD_RECORD_ENTRIES < u8NumberOfBlockPeriodRecordEntries)
        {
            return(E_ZCL_ERR_PARAMETER_RANGE);
        }
    }
    else
    {
        if(SE_PRICE_NUMBER_OF_CLIENT_BLOCK_PERIOD_RECORD_ENTRIES < u8NumberOfBlockPeriodRecordEntries)
        {
            return(E_ZCL_ERR_PARAMETER_RANGE);
        }
    }
#endif
    // cluster data
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   NULL,
                                   eSE_PriceCommandHandler);    

    psClusterInstance->pvEndPointCustomStructPtr = (void *)psCustomDataStructure;
    // initialise pointer
    psPrice_Common = &psCustomDataStructure->sPrice_Common;

    // clear
    memset(psPrice_Common, 0, sizeof(tsPrice_Common));
    psPrice_Common->sReceiveEventAddress.bInitialised = FALSE;

    // initialise the c/b structures
    psPrice_Common->sPriceCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psPrice_Common->sPriceCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterInstance->psClusterDefinition->u16ClusterEnum;
    psPrice_Common->sPriceCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psPrice_Common->sPriceCallBackMessage;
    psPrice_Common->sPriceCustomCallBackEvent.psClusterInstance = psClusterInstance;

    // add custom data for Tier Records
    psCustomDataStructure->u8NumberOfRecordEntries = u8NumberOfRecordEntries;
    psCustomDataStructure->psPublishPriceRecord =  psPublishPriceRecord;

#ifdef PRICE_CONVERSION_FACTOR
    psCustomDataStructure->u8NumberOfConversionRecordEntries =  u8NumberOfConversionFactorRecordEntries;
    psCustomDataStructure->psPublishConversionRecord =  psConversionFactorRecord;

    vDLISTinitialise(&psCustomDataStructure->lConversionFactorAllocList);
    vDLISTinitialise(&psCustomDataStructure->lConversionFactorDeAllocList);
#endif

#ifdef  PRICE_CALORIFIC_VALUE
    psCustomDataStructure->u8NumberOfCalorificValueEntries =  u8NumberOfCalorificValueRecordEntries;
    psCustomDataStructure->psPublishCalorificValueRecord =  psCalorificValueRecord;

    vDLISTinitialise(&psCustomDataStructure->lCalorificValueAllocList);
    vDLISTinitialise(&psCustomDataStructure->lCalorificValueDeAllocList);

#endif

#ifdef BLOCK_CHARGING
    // add custom data for Block Period Records
    psCustomDataStructure->u8NumberOfBlockRecordEntries = u8NumberOfBlockPeriodRecordEntries;
    psCustomDataStructure->psPublishBlockPeriodRecord =  psPublishBlockPeriodRecord;
#endif /* BLOCK_CHARGING */

    /* initialise lists for Tier */
    vDLISTinitialise(&psCustomDataStructure->lPriceAllocList);
    vDLISTinitialise(&psCustomDataStructure->lPriceDeAllocList);

#ifdef BLOCK_CHARGING
    /* initialise lists for Block Period */
    vDLISTinitialise(&psCustomDataStructure->lBlockPeriodAllocList);
    vDLISTinitialise(&psCustomDataStructure->lBlockPeriodDeAllocList);
#endif /* BLOCK_CHARGING */

    for(i=0; i< u8NumberOfRecordEntries; i++)
    {
        /* add all header slots to the to free list */
        vDLISTaddToTail(&psCustomDataStructure->lPriceDeAllocList, (DNODE *)&psCustomDataStructure->psPublishPriceRecord[i]);
        // don't set PRICE_PROVIDER_ID
        // set up rate label strings
        psCustomDataStructure->psPublishPriceRecord[i].sPublishPriceCmdPayload.sRateLabel.pu8Data = &pau8RateLabel[i*SE_PRICE_SERVER_MAX_STRING_LENGTH];
        psCustomDataStructure->psPublishPriceRecord[i].sPublishPriceCmdPayload.sRateLabel.u8MaxLength = SE_PRICE_SERVER_MAX_STRING_LENGTH;
    }

#ifdef BLOCK_CHARGING
    for(i=0; i< u8NumberOfBlockPeriodRecordEntries; i++)
    {
        /* add all header slots to the to free list */
        vDLISTaddToTail(&psCustomDataStructure->lBlockPeriodDeAllocList, (DNODE *)&psCustomDataStructure->psPublishBlockPeriodRecord[i]);
    }
#endif /* BLOCK_CHARGING */


#ifdef PRICE_CONVERSION_FACTOR
    for(i=0; i< u8NumberOfConversionFactorRecordEntries; i++)
    {
        /* add all header slots to the to free list */
        vDLISTaddToTail(&psCustomDataStructure->lConversionFactorDeAllocList, (DNODE *)&psCustomDataStructure->psPublishConversionRecord[i]);
    }
#endif /* PRICE CONVERSION FACTOR */


#ifdef PRICE_CALORIFIC_VALUE
    for(i=0; i< u8NumberOfCalorificValueRecordEntries; i++)
    {
        /* add all header slots to the to free list */
        vDLISTaddToTail(&psCustomDataStructure->lCalorificValueDeAllocList, (DNODE *)&psCustomDataStructure->psPublishCalorificValueRecord[i]);
    }
#endif /* BLOCK_CHARGING */

    /* Tier Price Price Label Set (D.4.2.2.1) */
#if (CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT != 0)
    {
        uint8 i;
        for(i =0; i<CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT; i++)
        {
            ((tsCLD_Price *)pvEndPointSharedStructPtr)->asTierPriceLabel[i].u8MaxLength = SE_PRICE_SERVER_MAX_STRING_LENGTH;
            ((tsCLD_Price *)pvEndPointSharedStructPtr)->asTierPriceLabel[i].u8Length = 0;
            ((tsCLD_Price *)pvEndPointSharedStructPtr)->asTierPriceLabel[i].pu8Data = ((tsCLD_Price *)pvEndPointSharedStructPtr)->au8TierPriceLabel[i];
        }
    }
#endif

    /* Block Period Set (D.4.2.2.3) */
#ifdef CLD_P_ATTR_START_OF_BLOCK_PERIOD
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->utctStartOfBlockPeriod = 0;
#endif

#ifdef CLD_P_ATTR_BLOCK_PERIOD_DURATION
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->u24BlockPeriodDuration = 0;
#endif

#ifdef CLD_P_ATTR_THRESHOLD_MULTIPLIER
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->u24ThresholdMultiplier = 1;
#endif

#ifdef CLD_P_ATTR_THRESHOLD_DIVISOR
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->u24ThresholdDivisor = 1;
#endif

    /* Commodity Set Set (D.4.2.2.4) */
#ifdef CLD_P_ATTR_COMMODITY_TYPE
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->e8CommodityType = 0;
#endif

#ifdef CLD_P_ATTR_STANDING_CHARGE
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->u32StandingCharge = 0;
#endif

// Billing period - new for RIB

#ifdef CLD_P_ATTR_START_OF_BILLING_PERIOD
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->utctStartOfBillingPeriod = 0;
#endif

#ifdef CLD_P_ATTR_BILLING_PERIOD_DURATION
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->u24BillingPeriodDuration = 0;
#endif

#ifdef CLD_P_ATTR_CONVERSION_FACTOR
    ((tsCLD_Price *)pvEndPointSharedStructPtr)-> u32ConversionFactor = 1;
#endif

#ifdef CLD_P_ATTR_CONVERSION_FACTOR_TRAILING_DIGIT
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->b8ConversionFactorTrailingDigit = 0x70;
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->u32CalorificValue = 0x2625A00;
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE_UNIT
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->e8CalorificValueUnit = 0x1;
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE_TRAILING_DIGIT
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->b8CalorificValueTrailingDigit = 0x60;
#endif
#ifdef CLD_P_CLIENT_ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->u8ClientIncreaseRandomize = 0x05;
#endif
#ifdef CLD_P_CLIENT_ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES
    ((tsCLD_Price *)pvEndPointSharedStructPtr)->u8ClientDecreaseRandomize = 0x0F;
#endif


    //  register timer
    return eSE_PriceRegisterPriceTimeServer();

}
#ifdef SE_PRICE_SERVER_AUTO_UPDATES_PRICE_FROM_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED

/****************************************************************************
 *
 * NAME: vSE_PriceLoadBlockConsumptionPointers
 *
 * DESCRIPTION:
 * Registers pointers to enable a publish price to be generated
 * with the new block's price when a block threshold is crossed.
 *
 * PARAMETERS:  Name                            Usage
 *              psPriceCustomDataStructure,     Pointer to the price cluster's custom data structure.
 *              psPriceAttributes,              Pointer to the shared attribute structure of the price cluster
 * pu48CurrentBlockPeriodConsumptionDelivered,  Pointer to the shared attribute value of the CurrentBlockPeriodConsumptionDelivered attribute in the metering cluster
 * pu24MeteringMultiplier,                      Pointer to the shared attribute value of the Multiplier formatting attribute in the metering cluster
 * pu24MeteringDivisor                          Pointer to the shared attribute value of the Divisor formatting attribute in the metering cluster
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC  void vSE_PriceLoadBlockConsumptionPointers(
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
    tsCLD_Price *psPriceAttributes,
    zuint48 *pu48CurrentBlockPeriodConsumptionDelivered,
    zuint24 *pu24MeteringMultiplier,
    zuint24 *pu24MeteringDivisor)
{
    psPriceCustomDataStructure->psPriceAttributes = psPriceAttributes;
    psPriceCustomDataStructure->pu48CurrentBlockPeriodConsumptionDelivered = pu48CurrentBlockPeriodConsumptionDelivered;
    psPriceCustomDataStructure->pu24MeteringMultiplier = pu24MeteringMultiplier;
    psPriceCustomDataStructure->pu24MeteringDivisor = pu24MeteringDivisor;
}

#endif
