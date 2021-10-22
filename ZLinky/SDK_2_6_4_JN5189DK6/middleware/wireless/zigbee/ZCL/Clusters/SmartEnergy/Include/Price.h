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
 * COMPONENT:          price.h
 *
 * DESCRIPTION:        The API for the Price Cluster
 *
 *****************************************************************************/

#ifndef  PRICE_H_INCLUDED
#define  PRICE_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"
#include "dlist.h"

#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"

/****************************************************************************/
/***        Macro Definitions that may be overridden in zcl_options.h     ***/
/****************************************************************************/
#ifndef SE_CLUSTER_ID_PRICE
#define SE_CLUSTER_ID_PRICE                                            0x0700
#endif

#ifndef SE_PRICE_NUMBER_OF_SERVER_PRICE_RECORD_ENTRIES
#define SE_PRICE_NUMBER_OF_SERVER_PRICE_RECORD_ENTRIES                (5)
#endif

#ifndef SE_PRICE_NUMBER_OF_SERVER_BLOCK_PERIOD_RECORD_ENTRIES
#define SE_PRICE_NUMBER_OF_SERVER_BLOCK_PERIOD_RECORD_ENTRIES         (2)
#endif


#ifndef SE_PRICE_NUMBER_OF_CLIENT_PRICE_RECORD_ENTRIES
#define SE_PRICE_NUMBER_OF_CLIENT_PRICE_RECORD_ENTRIES                 (2)
#endif

#ifndef SE_PRICE_NUMBER_OF_CLIENT_BLOCK_PERIOD_RECORD_ENTRIES
#define SE_PRICE_NUMBER_OF_CLIENT_BLOCK_PERIOD_RECORD_ENTRIES          (2)
#endif

#ifndef SE_PRICE_NUMBER_OF_CONVERSION_FACTOR_ENTRIES
#define SE_PRICE_NUMBER_OF_CONVERSION_FACTOR_ENTRIES                   (2)
#endif

#ifndef SE_PRICE_NUMBER_OF_CALORIFIC_VALUE_ENTRIES
#define SE_PRICE_NUMBER_OF_CALORIFIC_VALUE_ENTRIES                     (2)
#endif


#ifndef CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT
#define CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT                          (0)
#endif

#ifndef CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT
#define CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT                           (0)
#endif

#ifndef CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT
#define CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT                      (0)
#endif

#ifndef CLD_P_ATTR_NUM_OF_TIERS_PRICE
#define CLD_P_ATTR_NUM_OF_TIERS_PRICE                                  (0)
#endif

#ifndef CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE
#define CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE                    (0)
#endif

#if(CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT > 15)||(CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT > 15)||(CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT > 16)||(CLD_P_ATTR_NUM_OF_TIERS_PRICE > 15)||(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE > 16)
#error   Number of price attributes are out of range
#endif

#if(CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT != 0)&&((CLD_P_ATTR_NUM_OF_TIERS_PRICE != 0)||(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
#error  price attributes are conflicted
#endif

#if((CLD_P_ATTR_NUM_OF_TIERS_PRICE != 0)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE == 0))
#error  price attributes are conflicted
#endif

#ifndef PRICE_CHARGING_MODE
#define PRICE_CHARGING_MODE 1
#endif /* PRICE_CHARGING_MODE */

/* Charging mode macros */
#if PRICE_CHARGING_MODE == 1

#define TOU_CHARGING

#elif PRICE_CHARGING_MODE == 2

#define BLOCK_CHARGING

#elif PRICE_CHARGING_MODE == 3

#define TOU_CHARGING
#define BLOCK_CHARGING

#endif

#define CLD_P_MAX_BLOCKS_PER_TIER                                    (0x10)        /* MAX blocks in each tier */


#define ATTR_TIER_PRICE_LABEL_MAX_COUNT                    (CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT)
#if (CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT != 0)
    #define ATTR_BLOCK_THRESHOLD_MAX_COUNT                 (CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT)
#else
    #define ATTR_BLOCK_THRESHOLD_MAX_COUNT                (0)
#endif
#ifdef CLD_P_ATTR_START_OF_BLOCK_PERIOD
    #define ATTR_START_OF_BLOCK_PERIOD                    (1)
#else
    #define ATTR_START_OF_BLOCK_PERIOD                    (0)
#endif

#ifdef CLD_P_ATTR_BLOCK_PERIOD_DURATION
    #define ATTR_BLOCK_PERIOD_DURATION                    (1)
#else
    #define ATTR_BLOCK_PERIOD_DURATION                    (0)
#endif

#ifdef CLD_P_ATTR_THRESHOLD_MULTIPLIER
    #define ATTR_THRESHOLD_MULTIPLIER                     (1)
#else
    #define ATTR_THRESHOLD_MULTIPLIER                     (0)
#endif

#ifdef CLD_P_ATTR_THRESHOLD_DIVISOR
    #define ATTR_THRESHOLD_DIVISOR                        (1)
#else
    #define ATTR_THRESHOLD_DIVISOR                        (0)
#endif

#ifdef CLD_P_ATTR_COMMODITY_TYPE
    #define ATTR_COMMODITY_TYPE                           (1)
#else
    #define ATTR_COMMODITY_TYPE                           (0)
#endif

#ifdef CLD_P_ATTR_STANDING_CHARGE
    #define ATTR_STANDING_CHARGE                          (1)
#else
    #define ATTR_STANDING_CHARGE                          (0)
#endif

#ifdef CLD_P_ATTR_CONVERSION_FACTOR
    #define ATTR_CONVERSION_FACTOR                        (1)
#else
    #define ATTR_CONVERSION_FACTOR                        (0)
#endif

#ifdef CLD_P_ATTR_CONVERSION_FACTOR_TRAILING_DIGIT
    #define ATTR_CONVERSION_FACTOR_TRAILING_DIGIT         (1)
#else
    #define ATTR_CONVERSION_FACTOR_TRAILING_DIGIT         (0)
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE
    #define ATTR_CALORIFIC_VALUE                          (1)
#else
    #define ATTR_CALORIFIC_VALUE                          (0)
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE_UNIT
    #define ATTR_CALORIFIC_VALUE_UNIT                     (1)
#else
    #define ATTR_CALORIFIC_VALUE_UNIT                     (0)
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE_TRAILING_DIGIT
    #define ATTR_CALORIFIC_VALUE_TRAILING_DIGIT           (1)
#else
    #define ATTR_CALORIFIC_VALUE_TRAILING_DIGIT           (0)
#endif


#define    ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT           (CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT)


#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 0)&&                           \
(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    #define    ATTR_TIER1_BLOCK1_PRICE                        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
  #define    ATTR_TIER1_BLOCK1_PRICE                        (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 1)&&  \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))

#define    ATTR_TIER2_BLOCK1_PRICE                           (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
#define    ATTR_TIER2_BLOCK1_PRICE                           (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 2)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
   #define    ATTR_TIER3_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER3_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 3)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
   #define    ATTR_TIER4_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER4_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 4)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    #define    ATTR_TIER5_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
    #define    ATTR_TIER5_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 5)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
   #define    ATTR_TIER6_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER6_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 6)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
   #define    ATTR_TIER7_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER7_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 7)&&  \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
   #define    ATTR_TIER8_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER8_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 8)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    #define    ATTR_TIER9_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER9_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 9)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    #define    ATTR_TIER10_BLOCK1_PRICE                      (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER10_BLOCK1_PRICE                      (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 10)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    #define    ATTR_TIER11_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER11_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 11)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    #define    ATTR_TIER12_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER12_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 12)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    #define    ATTR_TIER13_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER13_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 13)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
   #define    ATTR_TIER14_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER14_BLOCK1_PRICE                       (0)
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 14)&& \
        (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
   #define    ATTR_TIER15_BLOCK1_PRICE                       (CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE)
#else
   #define    ATTR_TIER15_BLOCK1_PRICE                       (0)
#endif

// Billing period - new for RIB
#ifdef CLD_P_ATTR_START_OF_BILLING_PERIOD
   #define    ATTR_START_OF_BILLING_PERIOD                   (1)
#else
   #define    ATTR_START_OF_BILLING_PERIOD                   (0)
#endif

#ifdef CLD_P_ATTR_BILLING_PERIOD_DURATION
   #define    ATTR_BILLING_PERIOD_DURATION                   (1)
#else
   #define    ATTR_BILLING_PERIOD_DURATION                   (0)
#endif


#ifdef CLD_P_CLIENT_ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES
    #define    ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES         (1)
#else
    #define    ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES         (0)    
#endif

#ifdef CLD_P_CLIENT_ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES
    #define    ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES         (1)
#else
    #define    ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES         (0)    
#endif

#ifdef CLD_P_CLIENT_ATTR_COMMODITY_TYPE
    #define    CLIENT_ATTR_COMMODITY_TYPE                      (1)
#else
    #define    CLIENT_ATTR_COMMODITY_TYPE                      (0)    
#endif

#define CLD_PRICE_MAX_NUMBER_OF_ATTRIBUTE                    \
    ((ATTR_TIER_PRICE_LABEL_MAX_COUNT)        +           \
    (ATTR_BLOCK_THRESHOLD_MAX_COUNT)          +           \
    (ATTR_START_OF_BLOCK_PERIOD)              +           \
    (ATTR_BLOCK_PERIOD_DURATION)              +           \
    (ATTR_THRESHOLD_MULTIPLIER)               +           \
    (ATTR_THRESHOLD_DIVISOR)                  +           \
    (ATTR_COMMODITY_TYPE)                     +           \
    (ATTR_STANDING_CHARGE)                    +           \
    (ATTR_CONVERSION_FACTOR)                  +           \
    (ATTR_CONVERSION_FACTOR_TRAILING_DIGIT)   +           \
    (ATTR_CALORIFIC_VALUE)                    +           \
    (ATTR_CALORIFIC_VALUE_UNIT)               +           \
    (ATTR_CALORIFIC_VALUE_TRAILING_DIGIT)      +           \
    (ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT)      +           \
    (ATTR_TIER1_BLOCK1_PRICE)                  +           \
    (ATTR_TIER2_BLOCK1_PRICE)                  +           \
    (ATTR_TIER3_BLOCK1_PRICE)                 +           \
    (ATTR_TIER4_BLOCK1_PRICE)                  +           \
    (ATTR_TIER5_BLOCK1_PRICE)                 +           \
    (ATTR_TIER6_BLOCK1_PRICE)                 +           \
    (ATTR_TIER7_BLOCK1_PRICE)                 +           \
    (ATTR_TIER8_BLOCK1_PRICE)                 +           \
    (ATTR_TIER9_BLOCK1_PRICE)                 +           \
    (ATTR_TIER10_BLOCK1_PRICE)                +           \
    (ATTR_TIER11_BLOCK1_PRICE)                +           \
    (ATTR_TIER12_BLOCK1_PRICE)                +           \
    (ATTR_TIER13_BLOCK1_PRICE)                +           \
    (ATTR_TIER14_BLOCK1_PRICE)                +           \
    (ATTR_TIER15_BLOCK1_PRICE)                +           \
    (ATTR_START_OF_BILLING_PERIOD)            +           \
    (ATTR_BILLING_PERIOD_DURATION)              +              \
    (ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES)      +              \
    (ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES)      +              \
    (CLIENT_ATTR_COMMODITY_TYPE))


/****************************************************************************/
/***        Macro Definitions for Price Commands                          ***/
/****************************************************************************/

#define SE_PRICE_SERVER_MAX_STRING_LENGTH         (12)

#define E_SE_PUBLISH_PRICE                        (0x00)
#define E_SE_PUBLISH_BLOCK_PERIOD                 (0x01)
#define E_SE_PUBLISH_CONVERSION_FACTOR            (0x02)
#define E_SE_PUBLISH_CALORIFIC_VALUE              (0x03)
#define E_SE_GET_CURRENT_PRICE                    (0x00)
#define E_SE_GET_SCHEDULED_PRICES                 (0x01)
#define E_SE_PRICE_ACK                            (0x02)
#define E_SE_GET_BLOCK_PERIOD                     (0x03)
#define E_SE_GET_CONVERSION_FACTOR                (0x04)
#define E_SE_GET_CALORIFIC_VALUE                  (0x05)
#define E_SE_PRICE_DURATION_UNTIL_CHANGED         (0xffff)
#define E_SE_BLOCK_PERIOD_DURATION_UNTIL_CHANGED  (0xffffff)
#define PRICE_ACK_REQUIRED_MASK                   (0x01)


/****************************************************************************/
/***        Type Definitions for Price Commands                           ***/
/****************************************************************************/


#ifdef PRICE_CALORIFIC_VALUE

typedef enum 
{
    E_SE_MEGA_JOULES_METER_CUBE     =  0x01,
    E_SE_MEGA_JOULES_KILOGRAM       =  0x02

} tsSE_PriceCalorificValueUnits;

#endif

typedef enum 
{
    E_SE_PRICE_PT_NO_TIER_RELATED = 0,
    E_SE_PRICE_PT_REFERENCE_TIER_1_PRICE_LABEL,
    E_SE_PRICE_PT_REFERENCE_TIER_2_PRICE_LABEL,
    E_SE_PRICE_PT_REFERENCE_TIER_3_PRICE_LABEL,
    E_SE_PRICE_PT_REFERENCE_TIER_4_PRICE_LABEL,
    E_SE_PRICE_PT_REFERENCE_TIER_5_PRICE_LABEL,
    E_SE_PRICE_PT_REFERENCE_TIER_6_PRICE_LABEL
} teSE_PriceTierSubField;

typedef enum 
{
    E_SE_PRICE_RT_NO_TIER_RELATED = 0,
    E_SE_PRICE_RT_USEAGE_ACC_IN_CURRENT_TIER_1_SUMMATION_DELIVERED_ATTRIBUTE,
    E_SE_PRICE_RT_USEAGE_ACC_IN_CURRENT_TIER_2_SUMMATION_DELIVERED_ATTRIBUTE,
    E_SE_PRICE_RT_USEAGE_ACC_IN_CURRENT_TIER_3_SUMMATION_DELIVERED_ATTRIBUTE,
    E_SE_PRICE_RT_USEAGE_ACC_IN_CURRENT_TIER_4_SUMMATION_DELIVERED_ATTRIBUTE,
    E_SE_PRICE_RT_USEAGE_ACC_IN_CURRENT_TIER_5_SUMMATION_DELIVERED_ATTRIBUTE,
    E_SE_PRICE_RT_USEAGE_ACC_IN_CURRENT_TIER_6_SUMMATION_DELIVERED_ATTRIBUTE
} teSE_PriceRegisterTierSubField;

typedef enum 
{
    E_SE_PRICE_AC_RESERVED = 0,
    E_SE_PRICE_AC_KG_OF_CO2_PER_UNIT_OF_MEASURE
} teSE_PriceAlternativeCosts;
typedef enum 
{
    E_SE_PRICE_OVERLAP =0x80,
    E_SE_PRICE_TABLE_NOT_YET_ACTIVE,
    E_SE_PRICE_DATA_OLD,
    E_SE_PRICE_NOT_FOUND,
    E_SE_PRICE_TABLE_NOT_FOUND,
    E_SE_PRICE_OVERFLOW,
    E_SE_PRICE_DUPLICATE,
    E_SE_PRICE_NO_TABLES,
    E_SE_PRICE_BLOCK_PERIOD_TABLE_NOT_YET_ACTIVE,
    E_SE_PRICE_NO_BLOCKS,
    E_SE_PRICE_NUMBER_OF_BLOCK_THRESHOLD_MISMATCH,
    E_SE_BLOCK_PERIOD_OVERFLOW,
    E_SE_BLOCK_PERIOD_DUPLICATE,
    E_SE_BLOCK_PERIOD_DATA_OLD,
    E_SE_BLOCK_PERIOD_OVERLAP,
    E_SE_PRICE_STATUS_ENUM_END
} teSE_PriceStatus;

typedef enum 
{
    E_SE_PRICE_TABLE_ADD =0x00,
    E_SE_PRICE_TABLE_ACTIVE,
    E_SE_PRICE_GET_CURRENT_PRICE_RECEIVED,
    E_SE_PRICE_TIME_UPDATE,
    E_SE_PRICE_ACK_RECEIVED,
    E_SE_PRICE_NO_PRICE_TABLES,
    E_SE_PRICE_READ_BLOCK_PRICING,
    E_SE_PRICE_BLOCK_PERIOD_TABLE_ACTIVE,
    E_SE_PRICE_NO_BLOCK_PERIOD_TABLES,
    E_SE_PRICE_BLOCK_PERIOD_ADD,
    E_SE_PRICE_READ_BLOCK_THRESHOLDS,
    E_SE_PRICE_CONVERSION_FACTOR_TABLE_ACTIVE,
    E_SE_PRICE_CONVERSION_FACTOR_ADD,
    E_SE_PRICE_CALORIFIC_VALUE_TABLE_ACTIVE,
    E_SE_PRICE_CALORIFIC_VALUE_ADD,
    E_SE_PRICE_CBET_ENUM_END

} teSE_PriceCallBackEventType;



/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
struct CLD_Price_tag;

#ifdef BLOCK_CHARGING
/* structure for No-tier block prices */
typedef struct {
    uint32 au32NoTierBlockPrices[CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT+1];
}tsSE_NoTierBlockPrices;

#if defined(TOU_CHARGING)
/* structure for Tier block prices */
typedef struct {
    uint32 au32TierBlockPrices[(CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT+1)* CLD_P_ATTR_NUM_OF_TIERS_PRICE];
}tsSE_TierBlockPrices;
#endif /* defined(TOU_CHARGING) */

/* structure for block thresholds */
typedef struct {
    uint8      u8NoOfBlockThreshold;
    zuint48    au48BlockThreholds[CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT];
}tsSE_BlockThresholds;
#endif /* BLOCK_CHARGING */

typedef enum 
{
    E_SE_PRICE_REQUESTOR_RX_ON_IDLE = 0x01 // LSB set
} teSE_PriceCommandOptions;

typedef struct {
    zutctime        utctStartTime;
    uint8           u8NoOfEvents;
}tsSE_PriceGetBlockPeriodCmdPayload;

typedef struct {

    uint32                u32IssuerEventId;
    uint32                u32StartTime;
    uint32                u32ConversionFactor;
    zbmap8                b8ConversionFactorTrailingDigit;

}tsSE_PricePublishConversionCmdPayload;

typedef struct {
    zenum8                e8CalorificValueUnit;
    zbmap8                b8CalorificValueTrailingDigit;
    uint32                u32IssuerEventId;
    uint32                u32StartTime;
    uint32                u32CalorificValue;


}tsSE_PricePublishCalorificValueCmdPayload;

// Structure Reordered for SE1.7
typedef struct {
    uint8               u8UnitOfMeasure;
    uint8               u8PriceTrailingDigitAndPriceTier;
    uint8               u8NumberOfPriceTiersAndRegisterTiers;
    uint8               u8PriceRatio;
    uint8               u8GenerationPriceRatio;
    uint8               u8AlternateCostUnit;
    uint8               u8AlternateCostTrailingDigit;
    uint8               u8NumberOfBlockThresholds;
    uint8               u8PriceControl;
    uint16              u16Currency;
    uint16              u16DurationInMinutes;
    uint32              u32ProviderId;
    uint32              u32IssuerEventId;
    uint32              u32StartTime;
    uint32              u32Price;
    uint32              u32GenerationPrice;
    uint32              u32AlternateCostDelivered;
    tsZCL_OctetString   sRateLabel;

} tsSE_PricePublishPriceCmdPayload;


typedef struct {
    uint32              u32ProviderId;
    uint32              u32IssuerEventId;
    uint32              u32BlockPeriodStartTime;
    uint32              u32BlockPeriodDurationInMins;
    zbmap8              b8NoOfTiersAndNoOfBlockThreshold;
    zbmap8              b8BlockPeriodControl;
}tsSE_PricePublishBlockPeriodCmdPayload;

typedef struct {
    uint32              u32ProviderId;
    uint32              u32IssuerEventId;
    uint32              u32PriceAckTime;
    uint8               u8Control;
} tsSE_PriceAckCmdPayload;

typedef struct {
    DNODE                               dllPriceNode;
    // add other stuff here
    tsSE_PricePublishPriceCmdPayload    sPublishPriceCmdPayload;
} tsSE_PricePublishPriceRecord;

#ifdef BLOCK_CHARGING
typedef struct {
    DNODE                                       dllBlockPeriodNode;
    tsSE_BlockThresholds                        sBlockThresholds;
    tsSE_PricePublishBlockPeriodCmdPayload      sPublishBlockPeriodCmdPayload;
}tsSE_PricePublishBlockPeriodRecord;
#endif /* BLOCK_CHARGING */


typedef struct{
    DNODE                                        dllConversionFactorNode;
    tsSE_PricePublishConversionCmdPayload        sPublishConversionCmdPayload;
}tsSE_PriceConversionFactorRecord;


typedef struct{
    DNODE                                        dllCalorificValueNode;
    tsSE_PricePublishCalorificValueCmdPayload    sPublishCalorificValueCmdPayload;
}tsSE_PriceCalorificValueRecord;

// Definition of Price Callback Event Structure
typedef struct {
   teSE_PriceStatus                             ePriceStatus;
} tsSE_PriceTableCommand;

typedef struct {
   teSE_PriceStatus                             ePriceStatus;
   uint8                                        u8NumberOfEntriesFree;
} tsSE_PriceTableTimeEvent;

typedef struct {
   teSE_PriceStatus                             eBlockPeriodStatus;
   uint8                                        u8NumberOfEntriesFree;
} tsSE_BlockPeriodTableTimeEvent;


typedef struct {
    teSE_PriceStatus                            eCalorificValueStatus;
    uint8                                       u8NumberOfEntriesFree;
} tsSE_CalorificValueTableTimeEvent;

typedef struct {
    teSE_PriceStatus                            eConversionFactorStatus;
    uint8                                       u8NumberOfEntriesFree;
} tsSE_ConversionFactorTableTimeEvent;

typedef struct {
    tsZCL_ReceiveEventAddress                   *psReceiveEventAddress;
    uint8                                       u8NumberOfPriceTiersAndRegisterTiers;
    uint8                                       u8NumberOfBlockThresholds;
} tsSE_PriceAttrReadInput;

typedef struct
{
    teSE_PriceCallBackEventType                 eEventType;
    uint32                                      u32CurrentTime;

    union {
        tsSE_PriceTableCommand                  sPriceTableCommand;     // For E_SE_PRICE_TABLE_ADD
        tsSE_PriceTableTimeEvent                sPriceTableTimeEvent;   // For E_SE_PRICE_TABLE_ACTIVE
        teSE_PriceCommandOptions                ePriceCommandOptions;   // For E_SE_PRICE_GET_CURRENT_PRICE_RECEIVED
        tsSE_PriceAckCmdPayload                 *psAckCmdPayload;       // For E_SE_PRICE_ACK_RECEIVED
        tsSE_PriceAttrReadInput                 sReadAttrInfo;          // for E_SE_PRICE_READ_BLOCK_PRICING
        tsSE_BlockPeriodTableTimeEvent          sBlockPeriodTableTimeEvent; /* FOR E_SE_PRICE_BLOCK_PERIOD_TABLE_ACTIVE */
        tsSE_ConversionFactorTableTimeEvent     sConversionFactorTableTimeEvent;
        tsSE_CalorificValueTableTimeEvent       sCalorificValueTableTimeEvent;

    } uMessage;

} tsSE_PriceCallBackMessage;

typedef struct
{
    tsZCL_ReceiveEventAddress                   sReceiveEventAddress;
    tsZCL_CallBackEvent                         sPriceCustomCallBackEvent;
    tsSE_PriceCallBackMessage                   sPriceCallBackMessage;
} tsPrice_Common;

// Structure Reordered for SE1.7
typedef struct {

    uint8                                       u8NumberOfRecordEntries;
#ifdef BLOCK_CHARGING
    uint8                                       u8NumberOfBlockRecordEntries;
#endif
#ifdef PRICE_CONVERSION_FACTOR
    uint8                                       u8NumberOfConversionRecordEntries;
#endif
#ifdef PRICE_CALORIFIC_VALUE
    uint8                                       u8NumberOfCalorificValueEntries;
#endif
    tsPrice_Common                              sPrice_Common;
    /* Tier Stuff added here */
    DLIST                                       lPriceAllocList;
    DLIST                                       lPriceDeAllocList;
    tsSE_PricePublishPriceRecord                *psPublishPriceRecord;
    uint32                                      u32ArrivalTimeOfStartNowEntry;
    uint16                                      u16ArrivalDurationOfStartNowEntry;
#ifdef BLOCK_CHARGING
    /* Block Period Stuff added here */
    DLIST                                       lBlockPeriodAllocList;
    DLIST                                       lBlockPeriodDeAllocList;
    tsSE_PricePublishBlockPeriodRecord          *psPublishBlockPeriodRecord;
    uint32                                      u32BlockPeriodArrivalTimeOfStartNowEntry;
    uint32                                      u32BlockPeriodArrivalDurationOfStartNowEntry;
#endif /* BLOCK_CHARGING */
#ifdef PRICE_CONVERSION_FACTOR
    DLIST                                       lConversionFactorAllocList;
    DLIST                                       lConversionFactorDeAllocList;
    tsSE_PriceConversionFactorRecord            *psPublishConversionRecord;
#endif
#ifdef PRICE_CALORIFIC_VALUE
    DLIST                                       lCalorificValueAllocList;
    DLIST                                       lCalorificValueDeAllocList;
    tsSE_PriceCalorificValueRecord              *psPublishCalorificValueRecord;
    uint32                                      u32CalorificArrivalTimeOfStartNowEntry;
#endif

#ifdef SE_PRICE_SERVER_AUTO_UPDATES_PRICE_FROM_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED
    struct CLD_Price_tag                        *psPriceAttributes;
    zuint48                                     *pu48CurrentBlockPeriodConsumptionDelivered;
    zuint24                                     *pu24MeteringMultiplier;
    zuint24                                     *pu24MeteringDivisor;
#endif
} tsSE_PriceCustomDataStructure;

/****************************************************************************/
/***        Type Definitions for Price Attributes                         ***/
/****************************************************************************/

/*    Attribute IDs */
typedef enum 
{
    /* Price Cluster Attribute Tier Price Label Set Attr Ids (D.4.2.2.1)*/
    E_CLD_P_ATTR_TIER_1_PRICE_LABEL = 0x0000,
    E_CLD_P_ATTR_TIER_2_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_3_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_4_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_5_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_6_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_7_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_8_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_9_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_10_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_11_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_12_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_13_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_14_PRICE_LABEL,
    E_CLD_P_ATTR_TIER_15_PRICE_LABEL,

    /* Price Cluster Attribute Block Threshold Set Attr IDs (D.4.2.2.2)*/
    E_CLD_P_ATTR_BLOCK1_THRESHOLD = 0x0100,
    E_CLD_P_ATTR_BLOCK2_THRESHOLD,
    E_CLD_P_ATTR_BLOCK3_THRESHOLD,
    E_CLD_P_ATTR_BLOCK4_THRESHOLD,
    E_CLD_P_ATTR_BLOCK5_THRESHOLD,
    E_CLD_P_ATTR_BLOCK6_THRESHOLD,
    E_CLD_P_ATTR_BLOCK7_THRESHOLD,
    E_CLD_P_ATTR_BLOCK8_THRESHOLD,
    E_CLD_P_ATTR_BLOCK9_THRESHOLD,
    E_CLD_P_ATTR_BLOCK10_THRESHOLD,
    E_CLD_P_ATTR_BLOCK11_THRESHOLD,
    E_CLD_P_ATTR_BLOCK12_THRESHOLD,
    E_CLD_P_ATTR_BLOCK13_THRESHOLD,
    E_CLD_P_ATTR_BLOCK14_THRESHOLD,
    E_CLD_P_ATTR_BLOCK15_THRESHOLD,

    /* Price Cluster Attribute Block Period Set Attr IDs (D.4.2.2.3)*/
    E_CLD_P_ATTR_START_OF_BLOCK_PERIOD = 0x0200,
    E_CLD_P_ATTR_BLOCK_PERIOD_DURATION,
    E_CLD_P_ATTR_THRESHOLD_MULTIPLIER,
    E_CLD_P_ATTR_THRESHOLD_DIVISOR,

    /* Price Cluster Attribute Commodity Set Attr IDs (D.4.2.2.4)*/
    E_CLD_P_ATTR_COMMODITY_TYPE = 0x0300,
    E_CLD_P_ATTR_STANDING_CHARGE,
    E_CLD_P_ATTR_CONVERSION_FACTOR,
    E_CLD_P_ATTR_CONVERSION_FACTOR_TRAILING_DIGIT,
    E_CLD_P_ATTR_CALORIFIC_VALUE,
    E_CLD_P_ATTR_CALORIFIC_VALUE_UNIT,
    E_CLD_P_ATTR_CALORIFIC_VALUE_TRAILING_DIGIT,

    /* Price Cluster Attribute Block Price Information Set Attr IDs (D.4.2.2.5)*/
    E_CLD_P_ATTR_NOTIER_BLOCK1_PRICE = 0x0400,
    E_CLD_P_ATTR_NOTIER_BLOCK2_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK3_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK4_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK5_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK6_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK7_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK8_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK9_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK10_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK11_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK12_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK13_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK14_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK15_PRICE,
    E_CLD_P_ATTR_NOTIER_BLOCK16_PRICE,


    E_CLD_P_ATTR_TIER1_BLOCK1_PRICE = 0x0410,
    E_CLD_P_ATTR_TIER1_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER1_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER2_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER2_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER3_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER3_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER4_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER4_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER5_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER5_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER6_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER6_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER7_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER7_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER8_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER8_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER9_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER9_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER10_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER10_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER11_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER11_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER12_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER12_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER13_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER13_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER14_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER14_BLOCK16_PRICE,

    E_CLD_P_ATTR_TIER15_BLOCK1_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK2_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK3_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK4_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK5_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK6_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK7_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK8_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK9_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK10_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK11_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK12_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK13_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK14_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK15_PRICE,
    E_CLD_P_ATTR_TIER15_BLOCK16_PRICE,

    E_CLD_P_ATTR_START_OF_BILLING_PERIOD = 0x700,
    E_CLD_P_ATTR_BILLING_PERIOD_DURATION,

    E_CLD_P_CLIENT_ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES = 0x0000,
    E_CLD_P_CLIENT_ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES,
    E_CLD_P_CLIENT_ATTR_COMMODITY_TYPE
} teCLD_SM_PriceAttributeID;

/* Instance Definition */
typedef struct CLD_Price_tag
{
    /* Tier Price Label Set (D.4.2.2.1) */
#if (CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT != 0)
    tsZCL_OctetString       asTierPriceLabel[CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT];
    uint8                   au8TierPriceLabel[CLD_P_ATTR_TIER_PRICE_LABEL_MAX_COUNT][SE_PRICE_SERVER_MAX_STRING_LENGTH];
#endif

    /* Block Threshold Set (D.4.2.2) */
#if (CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT != 0)
    zuint48                 au48BlockThreshold[CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT];
#endif
    /* Block Period Set (D.4.2.2.3) */
#ifdef CLD_P_ATTR_START_OF_BLOCK_PERIOD
    zutctime                utctStartOfBlockPeriod;
#endif

#ifdef CLD_P_ATTR_BLOCK_PERIOD_DURATION
    zuint24                 u24BlockPeriodDuration;
#endif

#ifdef CLD_P_ATTR_THRESHOLD_MULTIPLIER
    zuint24                 u24ThresholdMultiplier;
#endif

#ifdef CLD_P_ATTR_THRESHOLD_DIVISOR
    zuint24                 u24ThresholdDivisor;
#endif


    /* Commodity Set Set (D.4.2.2.4) */
#ifdef CLD_P_ATTR_COMMODITY_TYPE
    zenum8                  e8CommodityType;
#endif

#ifdef CLD_P_ATTR_STANDING_CHARGE
    zuint32                 u32StandingCharge;
#endif

#ifdef CLD_P_ATTR_CONVERSION_FACTOR
    zuint32                 u32ConversionFactor;
#endif

#ifdef CLD_P_ATTR_CONVERSION_FACTOR_TRAILING_DIGIT
    zbmap8                  b8ConversionFactorTrailingDigit;
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE
    zuint32                 u32CalorificValue;
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE_UNIT
    zenum8                  e8CalorificValueUnit;
#endif

#ifdef CLD_P_ATTR_CALORIFIC_VALUE_TRAILING_DIGIT
    zbmap8                  b8CalorificValueTrailingDigit;
#endif

    /* Block Price Information Set (D.4.2.2.5) */
#if (CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT != 0)
    zuint32                 au32NoTierBlockPrice[CLD_P_ATTR_NO_TIER_BLOCK_PRICES_MAX_COUNT];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 0)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier1BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 1)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier2BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 2)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier3BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 3)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier4BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 4)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier5BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 5)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier6BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 6)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier7BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 7)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier8BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 8)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier9BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 9)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier10BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 10)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier11BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 11)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier12BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 12)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier13BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 13)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier14BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#if ((CLD_P_ATTR_NUM_OF_TIERS_PRICE > 14)&&(CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE != 0))
    zuint32                 au32Tier15BlockPrice[CLD_P_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_PRICE];
#endif

#ifdef CLD_P_ATTR_START_OF_BILLING_PERIOD
    zutctime                utctStartOfBillingPeriod;
#endif

#ifdef CLD_P_ATTR_BILLING_PERIOD_DURATION
    zuint24                 u24BillingPeriodDuration;
#endif
#ifdef CLD_P_CLIENT_ATTR_PRICE_INCREASE_RANDOMIZE_MINUTES
    uint8                 u8ClientIncreaseRandomize;
#endif
#ifdef CLD_P_CLIENT_ATTR_PRICE_DECREASE_RANDOMIZE_MINUTES
    uint8                 u8ClientDecreaseRandomize;
#endif
#ifdef CLD_P_CLIENT_ATTR_COMMODITY_TYPE
    zenum8                  e8ClientCommodityType;
#endif


} tsCLD_Price;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eSE_PriceCreate(
                    bool_t                              bIsServer,
                    uint8                               u8NumberOfRecordEntries,
                    uint8                               *pu8AttributeControlBits,
                    uint8                               *pau8RateLabel,
                    tsZCL_ClusterInstance               *psClusterInstance,
                    tsZCL_ClusterDefinition             *psClusterDefinition,
                    tsSE_PriceCustomDataStructure       *psCustomDataStructure,
                    tsSE_PricePublishPriceRecord        *psPublishPriceRecord,
                    void                                *pvEndPointSharedStructPtr
#ifdef BLOCK_CHARGING
                    ,tsSE_PricePublishBlockPeriodRecord *psPublishBlockPeriodRecord,
                    uint8                               u8NumberOfBlockPeriodRecordEntries
#endif
#ifdef PRICE_CONVERSION_FACTOR
                    ,tsSE_PriceConversionFactorRecord   *psConversionFactorRecord,
                    uint8                               u8NumberOfConversionFactorRecordEntries
#endif
#ifdef PRICE_CALORIFIC_VALUE
                    ,tsSE_PriceCalorificValueRecord     *psCalorificValueRecord,
                    uint8                               u8NumberOfCalorificValueRecordEntries
#endif
);

PUBLIC teSE_PriceStatus eSE_PriceGetScheduledPricesSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint32                      u32StartTime,
                    uint8                       u8NumberOfEvents);

PUBLIC teSE_PriceStatus eSE_PriceGetCurrentPriceSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    teSE_PriceCommandOptions    ePriceCommandOptions);

PUBLIC teSE_PriceStatus eSE_PriceGetBlockPeriodSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    tsSE_PriceGetBlockPeriodCmdPayload    *psGetBlockPeriodCmdPayload);

PUBLIC teSE_PriceStatus eSE_PriceAddPriceEntry(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    bool_t                      bOverwritePrevious,
                    tsSE_PricePublishPriceCmdPayload    *psPricePayload,
                    uint8                      *pu8TransactionSequenceNumber);

PUBLIC teSE_PriceStatus eSE_PriceAddPriceEntryToClient(
                    uint8                               u8SourceEndPointId,
                    bool_t                              bOverwritePrevious,
                    tsSE_PricePublishPriceCmdPayload    *psPricePayload);

#ifdef BLOCK_CHARGING
PUBLIC teSE_PriceStatus eSE_PriceAddBlockPeriodEntry(
                    uint8                                   u8SourceEndPointId,
                    uint8                                   u8DestinationEndPointId,
                    tsZCL_Address                           *psDestinationAddress,
                    bool_t                                  bOverwritePrevious,
                    tsSE_PricePublishBlockPeriodCmdPayload  *psPublishBlockPeriodCmdPayload,
                    tsSE_BlockThresholds                    *psBlockThresholds,
                    uint8                                   *pu8TransactionSequenceNumber);

PUBLIC teSE_PriceStatus eSE_PriceAddBlockPeriodEntryToClient(
                    uint8                                       u8SourceEndPointId,
                    bool_t                                      bOverwritePrevious,
                    tsSE_PricePublishBlockPeriodCmdPayload        *psBlockPeriodPayload);
#endif /* BLOCK_CHARGING */

PUBLIC teSE_PriceStatus eSE_PriceGetPriceEntry(
                    uint8                               u8SourceEndPointId,
                    bool_t                              bIsServer,
                    uint8                               u8TableIndex,
                    tsSE_PricePublishPriceCmdPayload    **psPricePayload);

#ifdef BLOCK_CHARGING
PUBLIC teSE_PriceStatus eSE_PriceGetBlockPeriodEntry(
                    uint8                                       u8SourceEndPointId,
                    bool_t                                      bIsServer,
                    uint8                                       u8tableIndex,
                    tsSE_PricePublishBlockPeriodCmdPayload      **ppsPublishBlockPeriodCmdPayload );
#endif /* BLOCK_CHARGING */


PUBLIC teSE_PriceStatus eSE_PriceRemovePriceEntry(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32StartTime);

PUBLIC teSE_PriceStatus eSE_PriceRemoveBlockPeriodEntry(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32StartTime);

PUBLIC teSE_PriceStatus eSE_PriceDoesPriceEntryExist(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32StartTime);

PUBLIC teSE_PriceStatus eSE_PriceDoesBlockPeriodEntryExist(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32StartTime);

PUBLIC teSE_PriceStatus eSE_PriceClearAllPriceEntries(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer);

PUBLIC teSE_PriceStatus eSE_PriceClearAllBlockPeriodEntries(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer);

PUBLIC  teSE_PriceStatus eSE_AddConversionFactorUsingPointer(
            uint8                                   u8SourceEndPointId,
			bool_t                                    bIsServer,
            tsSE_PriceCustomDataStructure           *psPriceCustomDataStructure,
            bool_t                                  bOverwritePrevious,
            tsSE_PricePublishConversionCmdPayload   *psPublishConversionCmdPayload);

PUBLIC teSE_PriceStatus eSE_AddCalorificValueEntryUsingPointer(
            uint8                                       u8SourceEndPointId,
			bool_t                                        bIsServer,
            tsSE_PriceCustomDataStructure               *psPriceCustomDataStructure,
            bool_t                                      bOverwritePrevious,
            tsSE_PricePublishCalorificValueCmdPayload   *psPublishCalorificValueCmdPayload);

PUBLIC teSE_PriceStatus eSE_AddConversionFactorEntryUsingPointer(
            uint8                                       u8SourceEndPointId,
			bool_t                                        bIsServer,
            tsSE_PriceCustomDataStructure               *psPriceCustomDataStructure,
            bool_t                                      bOverwritePrevious,
            tsSE_PricePublishConversionCmdPayload       *psPublishConversionCmdPayload);

#ifdef BLOCK_CHARGING
PUBLIC teSE_PriceStatus eSE_PriceAddNoTierBlockPrices(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8NoOfNoTierBlocks,
                    tsSE_NoTierBlockPrices     *psNoTierBlockPrices);

#if defined(TOU_CHARGING)
PUBLIC teSE_PriceStatus eSE_PriceAddTierBlockPrices(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8NoOfTiers,
                    uint8                       u8NoOfBlocks,
                    tsSE_TierBlockPrices       *psTierBlockPrices);
#endif /* (TOU_CHARGING) */

PUBLIC teSE_PriceStatus eSE_PriceAddBlockThresholds(
                    uint8                       u8SourceEndPointId,
                    tsSE_BlockThresholds       *psBlockThresholds);
#endif /* BLOCK_CHARGING */

#ifdef SE_PRICE_SERVER_AUTO_UPDATES_PRICE_FROM_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED
PUBLIC void vSE_PriceLoadBlockConsumptionPointers(
                    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure,
                    tsCLD_Price                *psPriceAttributes,
                    zuint48                    *pu48CurrentBlockPeriodConsumptionDelivered,
                    zuint24                    *pu24Multiplier,
                    zuint24                    *pu24Divisor);
#endif

#ifdef PRICE_CALORIFIC_VALUE

PUBLIC teSE_PriceStatus eSE_PriceGetCalorificValueEntry(
                    uint8                                       u8SourceEndPointId,
                    bool_t                                      bIsServer,
                    uint8                                       u8tableIndex,
                    tsSE_PricePublishCalorificValueCmdPayload **ppsPublishCalorificValueCmdPayload );

PUBLIC teSE_PriceStatus eSE_PriceRemoveCalorificValueEntry(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32StartTime);

PUBLIC teSE_PriceStatus eSE_PriceDoesCalorificValueEntryExist(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer,
                    uint32                      u32StartTime);

PUBLIC teSE_PriceStatus eSE_PriceClearAllCalorificValueEntries(
                    uint8                       u8SourceEndPointId,
                    bool_t                      bIsServer);

PUBLIC teSE_PriceStatus eSE_PriceGetCalorificValueSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint32                      u32StartTime,
                    uint8                       u8NumberOfEvents);

#endif


#ifdef PRICE_CONVERSION_FACTOR

PUBLIC teSE_PriceStatus eSE_PublishConversionFactorReceive(
         ZPS_tsAfEvent                              *pZPSevent,
         tsSE_PricePublishConversionCmdPayload      *psPublishConversionFactorCmdPayload,
         uint8                                      *pu8TransactionSequenceNumber,
         uint32                                     *pu32CurrentTimeInMessage);

PUBLIC teSE_PriceStatus eSE_PricePublishConversionFactorSend(
         uint8                                      u8SourceEndPointId,
         uint8                                      u8DestinationEndPointId,
         tsZCL_Address                              *psDestinationAddress,
         uint8                                      *pu8TransactionSequenceNumber,
         tsSE_PricePublishConversionCmdPayload      *psPublishConversionFactorCmdPayload);

PUBLIC teSE_PriceStatus eSE_GetConversionFactorReceive(
         ZPS_tsAfEvent                  *pZPSevent,
         tsZCL_EndPointDefinition       *psEndPointDefinition,
         tsZCL_ClusterInstance          *psClusterInstance,
         uint8                          *pu8TransactionSequenceNumber,
         uint32                         *pu32StartTime,
         uint8                          *pu8NumberOfEvents);

PUBLIC teSE_PriceStatus eSE_PriceAddConversionFactorEntry(
         uint8                                          u8SourceEndPointId,
         uint8                                          u8DestinationEndPointId,
         tsZCL_Address                                  *psDestinationAddress,
         bool_t                                         bOverwritePrevious,
         tsSE_PricePublishConversionCmdPayload          *psPublishConversionCmdPayload,
         uint8                                          *pu8TransactionSequenceNumber);

PUBLIC teSE_PriceStatus eSE_PriceGetConversionFactorSend(
         uint8                          u8SourceEndPointId,
         uint8                          u8DestinationEndPointId,
         tsZCL_Address                  *psDestinationAddress,
         uint8                          *pu8TransactionSequenceNumber,
         uint32                         u32StartTime,
         uint8                          u8NumberOfEvents);

PUBLIC teSE_PriceStatus eSE_GetNumberOfFreeConversionFactorTableEntries(
         tsZCL_EndPointDefinition       *psEndPointDefinition,
         tsSE_PriceCustomDataStructure  *psPriceCustomDataStructure,
         uint8                          *pu8NumberOfEntries);
#endif






/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

extern const tsZCL_AttributeDefinition asCLD_PriceClusterAttributeDefinitions[];
extern tsZCL_ClusterDefinition sCLD_Price;
extern uint8 au8PriceClusterAttributeControlBits[];

#if defined __cplusplus
}
#endif

#endif  /* PRICE_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
