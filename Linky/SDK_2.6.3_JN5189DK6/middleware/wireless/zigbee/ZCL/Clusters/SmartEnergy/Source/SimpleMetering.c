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
 * MODULE:             Simple Metering Cluster
 *
 * COMPONENT:          SimpleMetering.c
 *
 * DESCRIPTION:        Simple Metering Cluster Definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "SimpleMetering.h"
#include "dbg.h"



#ifdef CLD_SIMPLE_METERING
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
const tsZCL_AttributeDefinition asCLD_SimpleMeteringClusterAttributeDefinitions[] = {

    /* Reading information attribute set attribute ID's (D.3.2.2.1) */
    {E_CLD_SM_ATTR_ID_CURRENT_SUMMATION_DELIVERED,          (E_ZCL_AF_RD|E_ZCL_AF_RP),     E_ZCL_UINT32,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u32CurrentSummationDelivered), 0}

#ifdef CLD_SM_ATTR_CURRENT_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_SUMMATION_RECEIVED,           E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentSummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_MAX_DEMAND_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_MAX_DEMAND_DELIVERED,         E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentMaxDemandDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_MAX_DEMAND_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_MAX_DEMAND_RECEIVED,          E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentMaxDemandReceived), 0}
#endif

#ifdef CLD_SM_ATTR_DFT_SUMMATION
    ,{E_CLD_SM_ATTR_ID_DFT_SUMMATION,                        E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48DFTSummation), 0}
#endif

#ifdef CLD_SM_ATTR_DAILY_FREEZE_TIME
    ,{E_CLD_SM_ATTR_ID_DAILY_FREEZE_TIME,                    E_ZCL_AF_RD,     E_ZCL_UINT16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16DailyFreezeTime), 0}
#endif

#ifdef CLD_SM_ATTR_POWER_FACTOR
    ,{E_CLD_SM_ATTR_ID_POWER_FACTOR,                         E_ZCL_AF_RD,     E_ZCL_INT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i8PowerFactor), 0}
#endif

#ifdef CLD_SM_ATTR_READING_SNAPSHOT_TIME
    ,{E_CLD_SM_ATTR_ID_READING_SNAPSHOT_TIME,                E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_SimpleMetering*)(0))->utctReadingSnapshotTime), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_MAX_DEMAND_DELIVERED_TIME
    ,{E_CLD_SM_ATTR_ID_CURRENT_MAX_DEMAND_DELIVERED_TIME,    E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_SimpleMetering*)(0))->utctCurrentMaxDemandDeliveredTime), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_MAX_DEMAND_RECEIVED_TIME
    ,{E_CLD_SM_ATTR_ID_CURRENT_MAX_DEMAND_RECEIVED_TIME,     E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_SimpleMetering*)(0))->utctCurrentMaxDemandReceivedTime), 0}
#endif

#ifdef CLD_SM_ATTR_DEFAULT_UPDATE_PERIOD
    ,{E_CLD_SM_ATTR_ID_DEFAULT_UPDATE_PERIOD,                 E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8DefaultUpdatePeriod), 0}
#endif

#ifdef CLD_SM_ATTR_FAST_POLL_UPDATE_PERIOD
    ,{E_CLD_SM_ATTR_ID_FAST_POLL_UPDATE_PERIOD,                 E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8FastPollUpdatePeriod), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED,     E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentBlockPeriodConsumptionDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_DAILY_CONSUMPTION_TARGET
    ,{E_CLD_SM_ATTR_ID_DAILY_CONSUMPTION_TARGET,     E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24DailyConsumptionTarget), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_BLOCK
    ,{E_CLD_SM_ATTR_ID_CURRENT_BLOCK,     E_ZCL_AF_RD,    E_ZCL_ENUM8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->e8CurrentBlock), 0}
#endif

#ifdef CLD_SM_SUPPORT_GET_PROFILE

#ifdef CLD_SM_ATTR_PROFILE_INTERVAL_PERIOD
    ,{E_CLD_SM_ATTR_ID_PROFILE_INTERVAL_PERIOD,             E_ZCL_AF_RD,    E_ZCL_ENUM8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->eProfileIntervalPeriod), 0}
#endif

#ifdef CLD_SM_ATTR_INTERVAL_READ_REPORTING_PERIOD
    ,{E_CLD_SM_ATTR_ID_INTERVAL_READ_REPORTING_PERIOD,    E_ZCL_AF_RD,    E_ZCL_UINT16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16IntervalReadReportingPeriod), 0}
#endif

#endif // CLD_SM_SUPPORT_GET_PROFILE

#ifdef CLD_SM_ATTR_PRESET_READING_TIME
    ,{E_CLD_SM_ATTR_ID_PRESET_READING_TIME,    E_ZCL_AF_RD,    E_ZCL_UINT16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16PresetReadingTime), 0}
#endif

#ifdef CLD_SM_ATTR_SUMMATION_DELIVERED_PER_REPORT
    ,{E_CLD_SM_ATTR_ID_SUMMATION_DELIVERED_PER_REPORT,    E_ZCL_AF_RD,    E_ZCL_UINT16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16SummationDeliveredPerReport), 0}
#endif

#ifdef CLD_SM_ATTR_FLOW_RESTRICTION
    ,{E_CLD_SM_ATTR_ID_FLOW_RESTRICTION,    E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8FlowRestriction), 0}
#endif

#ifdef CLD_SM_ATTR_SUPPLY_STATUS
    ,{E_CLD_SM_ATTR_ID_SUPPLY_STATUS,    E_ZCL_AF_RD,    E_ZCL_ENUM8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8SupplyStatus), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_INLET_ENERGY_CARRIER_SUMMATION
    ,{E_CLD_SM_ATTR_ID_CURRENT_INLET_ENERGY_CARRIER_SUMMATION,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentInletEnergyCarrierSummation), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_OUTLET_ENERGY_CARRIER_SUMMATION
    ,{E_CLD_SM_ATTR_ID_CURRENT_OUTLET_ENERGY_CARRIER_SUMMATION,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentOutletEnergyCarrierSummation), 0}
#endif

#ifdef CLD_SM_ATTR_INLET_TEMPERATURE
    ,{E_CLD_SM_ATTR_ID_INLET_TEMPERATURE,    E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24InletTemperature), 0}
#endif

#ifdef CLD_SM_ATTR_OUTLET_TEMPERATURE
    ,{E_CLD_SM_ATTR_ID_OUTLET_TEMPERATURE,    E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24OutletTemperature), 0}
#endif

#ifdef CLD_SM_ATTR_CONTROL_TEMPERATURE
    ,{E_CLD_SM_ATTR_ID_CONTROL_TEMPERATURE,    E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24ControlTemperature), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_INLET_ENERGY_CARRIER_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_INLET_ENERGY_CARRIER_DEMAND,    E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentInletEnergyCarrierDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_OUTLET_ENERGY_CARRIER_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_OUTLET_ENERGY_CARRIER_DEMAND,    E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentOutletEnergyCarrierDemand), 0}
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_BLOCK_PERIOD_CONSUMPTION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_BLOCK_PERIOD_CONSUMPTION_DELIVERED,     E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48PreviousBlockPeriodConsumptionDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_ACTIVE_REGISTER_TIER_DELIVERED
    ,{E_CLD_SM_ATTR_ID_ACTIVE_REGISTER_TIER_DELIVERED,     E_ZCL_AF_RD,    E_ZCL_OSTRING,    (uint32)(&((tsCLD_SimpleMetering*)(0))->sActiveRegisterTierDelivered), 0}
#endif

    /* Time Of Use Information attribute attribute ID's set (D.3.2.2.2) */
#ifdef CLD_SM_ATTR_CURRENT_TIER_1_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_1_SUMMATION_DELIVERED,   (E_ZCL_AF_RD|E_ZCL_AF_RP),    E_ZCL_UINT32,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u32CurrentTier1SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_1_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_1_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier1SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_2_SUMMATION_DELIVERED
   // ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_2_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier2SummationDelivered), 0}
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_2_SUMMATION_DELIVERED,   (E_ZCL_AF_RD|E_ZCL_AF_RP),    E_ZCL_UINT32,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u32CurrentTier2SummationDelivered), 0}

#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_2_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_2_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier2SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_3_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_3_SUMMATION_DELIVERED,   (E_ZCL_AF_RD|E_ZCL_AF_RP),    E_ZCL_UINT32,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u32CurrentTier3SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_3_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_3_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier3SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_4_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_4_SUMMATION_DELIVERED,   (E_ZCL_AF_RD|E_ZCL_AF_RP),    E_ZCL_UINT32,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u32CurrentTier4SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_4_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_4_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u64CurrentTier4SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_5_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_5_SUMMATION_DELIVERED,   (E_ZCL_AF_RD|E_ZCL_AF_RP),    E_ZCL_UINT32,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u32CurrentTier5SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_5_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_5_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier5SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_6_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_6_SUMMATION_DELIVERED,   (E_ZCL_AF_RD|E_ZCL_AF_RP),    E_ZCL_UINT32,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u32CurrentTier6SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_6_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_6_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier6SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_7_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_7_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier7SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_7_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_7_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier7SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_8_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_8_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier8SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_8_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_8_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier8SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_9_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_9_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier9SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_9_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_9_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier9SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_10_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_10_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier10SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_10_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_10_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier10SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_11_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_11_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier11SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_11_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_11_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier11SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_12_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_12_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier12SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_12_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_12_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier12SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_13_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_13_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier13SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_13_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_13_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier13SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_14_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_14_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier14SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_14_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_14_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier14SummationReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_15_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_15_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier15SummationDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_15_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_15_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentTier15SummationReceived), 0}
#endif

    /* Meter status attribute set attribute ID's (D.3.2.2.3) */
    ,{E_CLD_SM_ATTR_ID_STATUS,                               E_ZCL_AF_RD,     E_ZCL_BMAP8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8MeterStatus), 0}

#ifdef CLD_SM_ATTR_REMAINING_BATTERY_LIFE
    ,{E_CLD_SM_ATTR_ID_REMAINING_BATTERY_LIFE,    E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8RemainingBatteryLife), 0}
#endif

#ifdef CLD_SM_ATTR_HOURS_IN_OPERATION
    ,{E_CLD_SM_ATTR_ID_HOURS_IN_OPERATION,    E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24HoursInOperation), 0}
#endif

#ifdef CLD_SM_ATTR_HOURS_IN_FAULT
    ,{E_CLD_SM_ATTR_ID_HOURS_IN_FAULT,    E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24HoursInFault), 0}
#endif

    /* Formatting attribute set attribute ID's (D.3.2.2.4) */
    ,{E_CLD_SM_ATTR_ID_UNIT_OF_MEASURE,                      E_ZCL_AF_RD,    E_ZCL_ENUM8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->eUnitOfMeasure), 0}

#ifdef CLD_SM_ATTR_MULTIPLIER
    ,{E_CLD_SM_ATTR_ID_MULTIPLIER,                           E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24Multiplier), 0}
#endif

#ifdef CLD_SM_ATTR_DIVISOR
    ,{E_CLD_SM_ATTR_ID_DIVISOR,                              E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24Divisor), 0}
#endif

    ,{E_CLD_SM_ATTR_ID_SUMMATION_FORMATING,                  E_ZCL_AF_RD,     E_ZCL_BMAP8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8SummationFormatting), 0}

#ifdef CLD_SM_ATTR_DEMAND_FORMATING
    ,{E_CLD_SM_ATTR_ID_DEMAND_FORMATING,                     E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8DemandFormatting), 0}
#endif

#ifdef CLD_SM_ATTR_HISTORICAL_CONSUMPTION_FORMATTING
    ,{E_CLD_SM_ATTR_ID_HISTORICAL_CONSUMPTION_FORMATTING,    E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8HistoricalConsumptionFormatting), 0}
#endif

    ,{E_CLD_SM_ATTR_ID_METERING_DEVICE_TYPE,                 E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->eMeteringDeviceType), 0}
#ifdef CLD_SM_ATTR_SITE_ID
    ,{E_CLD_SM_ATTR_ID_SITE_ID,    E_ZCL_AF_RD,    E_ZCL_OSTRING,    (uint32)(&((tsCLD_SimpleMetering*)(0))->sSiteId), 0}
#endif

#ifdef CLD_SM_ATTR_METER_SERIAL_NUMBER
    ,{E_CLD_SM_ATTR_ID_METER_SERIAL_NUMBER,    E_ZCL_AF_RD,    E_ZCL_OSTRING,    (uint32)(&((tsCLD_SimpleMetering*)(0))->sMeterSerialNumber), 0}
#endif

#ifdef CLD_SM_ATTR_ENERGY_CARRIER_UNIT_OF_MEASURE
    ,{E_CLD_SM_ATTR_ID_ENERGY_CARRIER_UNIT_OF_MEASURE,    E_ZCL_AF_RD,    E_ZCL_ENUM8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->e8EnergyCarrierUnitOfMeasure), 0}
#endif

#ifdef CLD_SM_ATTR_ENERGY_CARRIER_SUMMATION_FORMATTING
    ,{E_CLD_SM_ATTR_ID_ENERGY_CARRIER_SUMMATION_FORMATTING,    E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8EnergyCarrierSummationFormatting), 0}
#endif

#ifdef CLD_SM_ATTR_ENERGY_CARRIER_DEMAND_FORMATTING
    ,{E_CLD_SM_ATTR_ID_ENERGY_CARRIER_DEMAND_FORMATTING,    E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8EnergyCarrierDemandFormatting), 0}
#endif

#ifdef CLD_SM_ATTR_TEMPERATURE_UNIT_OF_MEASURE
    ,{E_CLD_SM_ATTR_ID_TEMPERATURE_UNIT_OF_MEASURE,    E_ZCL_AF_RD,    E_ZCL_ENUM8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->e8TemperatureUnitOfMeasure), 0}
#endif

#ifdef CLD_SM_ATTR_TEMPERATURE_FORMATTING
    ,{E_CLD_SM_ATTR_ID_TEMPERATURE_FORMATTING,    E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8TemperatureFormatting), 0}
#endif

    /* ESP Historical Consumption set attribute ID's (D.3.2.2.5) */
#ifdef CLD_SM_ATTR_INSTANTANEOUS_DEMAND
    ,{E_CLD_SM_ATTR_ID_INSTANTANEOUS_DEMAND,                                     E_ZCL_AF_RD,     E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24InstantaneousDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_CONSUMPTION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_DAY_CONSUMPTION_DELIVERED,                        E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24CurrentDayConsumptionDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_CONSUMPTION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_DAY_CONSUMPTION_RECEIVED,                         E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24CurrentDayConsumptionReceived), 0}
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_CONSUMPTION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_DAY_CONSUMPTION_DELIVERED,                       E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24PreviousDayConsumptionDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_CONSUMPTION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_DAY_CONSUMPTION_RECEIVED,                        E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24PreviousDayConsumptionReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED,    E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_SimpleMetering*)(0))->utctCurrentPartialProfileIntervalStartTimeDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED,     E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_SimpleMetering*)(0))->utctCurrentPartialProfileIntervalStartTimeReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED,         E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24CurrentPartialProfileIntervalValueDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED,          E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24CurrentPartialProfileIntervalValueReceived), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_MAXIMUM_PRESSURE
    ,{E_CLD_SM_ATTR_ID_CURRENT_DAY_MAXIMUM_PRESSURE,                        E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentDayMaxPressure), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_MINIMUM_PRESSURE
    ,{E_CLD_SM_ATTR_ID_CURRENT_DAY_MINIMUM_PRESSURE,                        E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48CurrentDayMinPressure), 0}
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_MAXIMUM_PRESSURE
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_DAY_MAXIMUM_PRESSURE,                       E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48PreviousDayMaxPressure), 0}
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_MINIMUM_PRESSURE
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_DAY_MINIMUM_PRESSURE,                       E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48PreviousDayMinPressure), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_MAXIMUM_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_DAY_MAXIMUM_DEMAND,                          E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentDayMaxDemand), 0}
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_MAXIMUM_DEMAND
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_DAY_MAXIMUM_DEMAND,                         E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24PreviousDayMaxDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_MONTH_MAXIMUM_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_MONTH_MAXIMUM_DEMAND,                        E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentMonthMaxDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_YEAR_MAXIMUM_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_YEAR_MAXIMUM_DEMAND,                         E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentYearMaxDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND,           E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentDayMaxEnergyCarrierDemand), 0}
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND,          E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24PreviousDayMaxEnergyCarrierDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_MONTH_MAXIMUM_ENERGY_CARRIER_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_MONTH_MAXIMUM_ENERGY_CARRIER_DEMAND,          E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentMonthMaxEnergyCarrierDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_MONTH_MINIMUM_ENERGY_CARRIER_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_MONTH_MINIMUM_ENERGY_CARRIER_DEMAND,          E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentMonthMinEnergyCarrierDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_YEAR_MAXIMUM_ENERGY_CARRIER_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_YEAR_MAXIMUM_ENERGY_CARRIER_DEMAND,          E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentYearMaxEnergyCarrierDemand), 0}
#endif

#ifdef CLD_SM_ATTR_CURRENT_YEAR_MINIMUM_ENERGY_CARRIER_DEMAND
    ,{E_CLD_SM_ATTR_ID_CURRENT_YEAR_MINIMUM_ENERGY_CARRIER_DEMAND,          E_ZCL_AF_RD,    E_ZCL_INT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->i24CurrentYearMinEnergyCarrierDemand), 0}
#endif

    /* Load Profile attribute set attribute ID's (D.3.2.2.6) */
#ifdef CLD_SM_ATTR_MAX_NUMBER_OF_PERIODS_DELIVERED
    ,{E_CLD_SM_ATTR_ID_MAX_NUMBER_OF_PERIODS_DELIVERED,                     E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8MaxNumberOfPeriodsDelivered), 0}
#endif

    /* Supply Limit attribute set attribute ID's (D.3.2.2.7) */
#ifdef CLD_SM_ATTR_CURRENT_DEMAND_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_DEMAND_DELIVERED,                            E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24CurrentDemandDelivered), 0}
#endif

#ifdef CLD_SM_ATTR_DEMAND_LIMIT
    ,{E_CLD_SM_ATTR_ID_DEMAND_LIMIT,                                        E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24DemandLimit), 0}
#endif

#ifdef CLD_SM_ATTR_DEMAND_INTEGRATION_PERIOD
    ,{E_CLD_SM_ATTR_ID_DEMAND_INTEGRATION_PERIOD,                           E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8DemandIntegrationPeriod), 0}
#endif

#ifdef CLD_SM_ATTR_NUMBER_OF_DEMAND_SUBINTERVALS
    ,{E_CLD_SM_ATTR_ID_NUMBER_OF_DEMAND_SUBINTERVALS,                       E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8NumberOfDemandSubintervals), 0}
#endif

    /* Block Information attribute set attribute ID's (D.3.2.2.8) */
    /* No Tier Block Set */
#if (CLD_SM_ATTR_NO_TIER_BLOCK_CURRENT_SUMMATION_DELIVERED_MAX_COUNT != 0)
    ,{E_CLD_SM_ATTR_ID_CURRENT_NOTIER_BLOCK1_SUMMATION_DELIVERED,           E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentNoTierBlockSummationDelivered[0]), CLD_SM_ATTR_NO_TIER_BLOCK_CURRENT_SUMMATION_DELIVERED_MAX_COUNT -1}
#endif

    /* Tier1 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 0)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER1_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier1BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier2 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 1)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER2_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier2BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier3 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 2)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER3_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier3BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier4 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 3)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER4_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier4BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier5 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 4)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER5_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier5BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier6 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 5)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER6_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier6BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier7 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 6)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER7_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier7BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier8 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 7)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER8_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier8BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier9 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 8)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER9_BLOCK1_SUMMATION_DELIVERED,            E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier9BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier10 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 9)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER10_BLOCK1_SUMMATION_DELIVERED,           E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier10BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier11 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 10)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER11_BLOCK1_SUMMATION_DELIVERED,           E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier11BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier12 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 11)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER12_BLOCK1_SUMMATION_DELIVERED,           E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier12BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier13 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 12)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER13_BLOCK1_SUMMATION_DELIVERED,           E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier13BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier14 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 13)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER14_BLOCK1_SUMMATION_DELIVERED,           E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier14BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Tier15 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 14)&&(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER15_BLOCK1_SUMMATION_DELIVERED,           E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->au48CurrentTier15BlockSummationDelivered[0]), CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED -1}
#endif

    /* Alarm attribute set attribute ID's (D.3.2.2.9) */
#ifdef CLD_SM_ATTR_GENERIC_ALARM_MASK
    ,{E_CLD_SM_ATTR_ID_GENERIC_ALARM_MASK,                                  (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_BMAP16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16GenericAlarmMask), 0}
#endif

#ifdef CLD_SM_ATTR_ELECTRICITY_ALARM_MASK
    ,{E_CLD_SM_ATTR_ID_ELECTRICITY_ALARM_MASK,                              (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_BMAP32,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u32ElectricityAlarmMask), 0}
#endif

#ifdef CLD_SM_ATTR_PRESSURE_ALARM_MASK
    ,{E_CLD_SM_ATTR_ID_PRESSURE_ALARM_MASK,                                 (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_BMAP16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16PressureAlarmMask), 0}
#endif

#ifdef CLD_SM_ATTR_WATER_SPECIFIC_ALARM_MASK
    ,{E_CLD_SM_ATTR_ID_WATER_SPECIFIC_ALARM_MASK,                           (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_BMAP16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16WaterSpecificAlarmMask), 0}
#endif

#ifdef CLD_SM_ATTR_HEAT_AND_COOLING_ALARM_MASK
    ,{E_CLD_SM_ATTR_ID_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK,                (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_BMAP16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16HeatAndCoolingSpecificAlarmMask), 0}
#endif

#ifdef CLD_SM_ATTR_GAS_ALARM_MASK
    ,{E_CLD_SM_ATTR_ID_GAS_ALARM_MASK,                                      (E_ZCL_AF_RD|E_ZCL_AF_WR),    E_ZCL_BMAP16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16GasAlarmMask), 0}
#endif

    /* Manufacturer Specific Attributes */
#ifdef CLD_SM_ATTR_MAN_SPEC_AC_FREQUENCY
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_AC_FREQUENCY,                               (E_ZCL_AF_RD|E_ZCL_AF_MS),     E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8ManSpecACFrequency), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_RMS_VOLTAGE
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_RMS_VOLTAGE,                                (E_ZCL_AF_RD|E_ZCL_AF_MS),   E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecRMSVoltage), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_RMS_CURRENT
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_RMS_CURRENT,                                (E_ZCL_AF_RD|E_ZCL_AF_MS),   E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecRMSCurrent), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_ACTIVE_POWER
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_ACTIVE_POWER,                               (E_ZCL_AF_RD|E_ZCL_AF_MS),   E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecActivePower), 0}
#endif
    /* Manufacturer Specific Attributes: Calibration */
#ifdef CLD_SM_ATTR_MAN_SPEC_CALIBRATION_VPP
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_VPP,                            (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecCalibrationVpp), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_CALIBRATION_I1PP
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_I1PP,                           (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecCalibrationI1pp), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_CALIBRATION_I2PP
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_I2PP,                           (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecCalibrationI2pp), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_CALIBRATION_DELTAPHI1
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_DELTAPHI1,                      (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecCalibrationDeltaPhi1), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_CALIBRATION_DELTAPHI2
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_DELTAPHI2,                      (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),    E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecCalibrationDeltaPhi2), 0}
#endif

#ifdef CLD_SM_ATTR_MAN_SPEC_CALIBRATION_DIVISOR
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_DIVISOR,                        (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),     E_ZCL_UINT24,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u24ManSpecCalibrationDivisor), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_POWER_THRESHOLD
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_POWER_THRESHOLD,                            (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u48ManSpecPowerThreshold), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_UPDATE_INTERVAL
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_UPDATE_INTERVAL,                            (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),     E_ZCL_UINT16,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u16ManSpecUpdateInterval), 0}
#endif
#ifdef CLD_SM_ATTR_MAN_SPEC_RF_POWER_CONFIG
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_RF_POWER_CONFIG,                            (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_MS),     E_ZCL_UINT8,    (uint32)(&((tsCLD_SimpleMetering*)(0))->u8ManSpecRfPowerConfig), 0}
#endif
};

#if(CLD_SM_ATTR_NO_TIER_BLOCK_CURRENT_SUMMATION_DELIVERED_MAX_COUNT != 0)
#define CLD_SM_ATTR_NUM_OF_CURRENT_SUMMATION_DELIVERED         CLD_SM_ATTR_NO_TIER_BLOCK_CURRENT_SUMMATION_DELIVERED_MAX_COUNT - 1
#elif (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0)
#define CLD_SM_ATTR_NUM_OF_CURRENT_SUMMATION_DELIVERED         CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED*(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED - 1)
#else
#define CLD_SM_ATTR_NUM_OF_CURRENT_SUMMATION_DELIVERED         0
#endif

#define SM_NUM_OF_ATTRIBUTES      sizeof(asCLD_SimpleMeteringClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition) + \
                                  CLD_SM_ATTR_NUM_OF_CURRENT_SUMMATION_DELIVERED

#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
const tsZCL_CommandDefinition asCLD_SimpleMeteringClusterCommandDefinitions[] = {
    {E_CLD_SM_GET_PROFILE,                     E_ZCL_CF_RX},     /* Mandatory */
    {E_CLD_SM_REQUEST_FAST_POLL_MODE,          E_ZCL_CF_RX},     /* Mandatory */
    {E_CLD_SM_REQUEST_MIRROR_RESPONSE,         E_ZCL_CF_RX},     /* Mandatory */    
    {E_CLD_SM_MIRROR_REMOVED,                  E_ZCL_CF_RX},      /* Mandatory */ 
    
    {E_CLD_SM_GET_PROFILE_RESPONSE,            E_ZCL_CF_TX},     /* Mandatory */
    {E_CLD_SM_REQUEST_FAST_POLL_MODE_RESPONSE, E_ZCL_CF_TX},     /* Mandatory */
    {E_CLD_SM_REQUEST_MIRROR,                  E_ZCL_CF_TX}  
    
};
#endif                                  
tsZCL_ClusterDefinition sCLD_SimpleMetering = {
        SE_CLUSTER_ID_SIMPLE_METERING,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        SM_NUM_OF_ATTRIBUTES,
        (tsZCL_AttributeDefinition*)asCLD_SimpleMeteringClusterAttributeDefinitions,
        NULL
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
        ,
        (sizeof(asCLD_SimpleMeteringClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
        (tsZCL_CommandDefinition*)asCLD_SimpleMeteringClusterCommandDefinitions         
#endif        

};
#ifdef SM_SERVER
uint8 au8SimpleMeteringServerAttributeControlBits[SM_NUM_OF_ATTRIBUTES];
#endif

tsZCL_ClusterDefinition sCLD_SMClientNoAttribute = {
        SE_CLUSTER_ID_SIMPLE_METERING,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        0,
        NULL
};


#ifdef CLD_SM_SUPPORT_MIRROR
/*
 * Attribute Definition for Simple metering cluster
 * Required for Mirroring
 */
const tsZCL_AttributeDefinition asCLD_SMMirrorClusterAttributeDefinitions[] = {

    /* Reading information attribute set attribute ID's (D.3.2.2.1) */
    {E_CLD_SM_ATTR_ID_CURRENT_SUMMATION_DELIVERED,          E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentSummationDelivered), 0}

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_SUMMATION_RECEIVED,           E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentSummationReceived),0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_MAX_DEMAND_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_MAX_DEMAND_DELIVERED,         E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentMaxDemandDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_MAX_DEMAND_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_MAX_DEMAND_RECEIVED,          E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentMaxDemandReceived),0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_DFT_SUMMATION
    ,{E_CLD_SM_ATTR_ID_DFT_SUMMATION,                        E_ZCL_AF_RD,     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48DFTSummation), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_DAILY_FREEZE_TIME
    ,{E_CLD_SM_ATTR_ID_DAILY_FREEZE_TIME,                    E_ZCL_AF_RD,     E_ZCL_UINT16,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u16DailyFreezeTime), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_POWER_FACTOR
    ,{E_CLD_SM_ATTR_ID_POWER_FACTOR,                         E_ZCL_AF_RD,     E_ZCL_INT8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->i8PowerFactor), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_READING_SNAPSHOT_TIME
    ,{E_CLD_SM_ATTR_ID_READING_SNAPSHOT_TIME,                E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_Mirror_SM*)(0))->utctReadingSnapshotTime), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_MAX_DEMAND_DELIVERED_TIME
    ,{E_CLD_SM_ATTR_ID_CURRENT_MAX_DEMAND_DELIVERED_TIME,    E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_Mirror_SM*)(0))->utctCurrentMaxDemandDeliveredTime), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_MAX_DEMAND_RECEIVED_TIME
    ,{E_CLD_SM_ATTR_ID_CURRENT_MAX_DEMAND_RECEIVED_TIME,     E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_Mirror_SM*)(0))->utctCurrentMaxDemandReceivedTime), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_PROFILE_INTERVAL_PERIOD
    ,{E_CLD_SM_ATTR_ID_PROFILE_INTERVAL_PERIOD,             E_ZCL_AF_RD,    E_ZCL_ENUM8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->eProfileIntervalPeriod), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_INTERVAL_READ_REPORTING_PERIOD
    ,{E_CLD_SM_ATTR_ID_INTERVAL_READ_REPORTING_PERIOD,    E_ZCL_AF_RD,    E_ZCL_UINT16,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u32IntervalReadReportingPeriod), 0}
#endif

    /* Time Of Use Information attribute attribute ID's set (D.3.2.2.2) */
#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_1_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_1_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier1SummationDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_1_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_1_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier1SummationReceived), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_2_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_2_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier2SummationDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_2_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_2_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier2SummationReceived), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_3_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_3_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier3SummationDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_3_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_3_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier3SummationReceived), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_4_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_4_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier4SummationDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_4_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_4_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier4SummationReceived), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_5_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_5_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier5SummationDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_5_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_5_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier5SummationReceived), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_6_SUMMATION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_6_SUMMATION_DELIVERED,   E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier6SummationDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_TIER_6_SUMMATION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_TIER_6_SUMMATION_RECEIVED,    E_ZCL_AF_RD,    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48CurrentTier6SummationReceived), 0}
#endif

    /* Meter status attribute set attribute ID's (D.3.2.2.3) */
    ,{E_CLD_SM_ATTR_ID_STATUS,                               E_ZCL_AF_RD,     E_ZCL_BMAP8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8MeterStatus), 0}

    /* Formatting attribute set attribute ID's (D.3.2.2.4) */
    ,{E_CLD_SM_ATTR_ID_UNIT_OF_MEASURE,                      E_ZCL_AF_RD,    E_ZCL_ENUM8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->eUnitOfMeasure), 0}

#ifdef CLD_SM_MIRROR_ATTR_MULTIPLIER
    ,{E_CLD_SM_ATTR_ID_MULTIPLIER,                           E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24Multiplier), 0}
#endif
#ifdef CLD_SM_MIRROR_ATTR_DIVISOR
    ,{E_CLD_SM_ATTR_ID_DIVISOR,                              E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24Divisor), 0}
#endif

    ,{E_CLD_SM_ATTR_ID_SUMMATION_FORMATING,                  E_ZCL_AF_RD,     E_ZCL_BMAP8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8SummationFormatting), 0}

#ifdef CLD_SM_MIRROR_ATTR_DEMAND_FORMATING
    ,{E_CLD_SM_ATTR_ID_DEMAND_FORMATING,                     E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8DemandFormatting), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_HISTORICAL_CONSUMPTION_FORMATTING
    ,{E_CLD_SM_ATTR_ID_HISTORICAL_CONSUMPTION_FORMATTING,    E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8HistoricalConsumptionFormatting), 0}
#endif

    ,{E_CLD_SM_ATTR_ID_METERING_DEVICE_TYPE,                 E_ZCL_AF_RD,    E_ZCL_BMAP8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8MeteringDeviceType), 0}

#ifdef CLD_SM_MIRROR_ATTR_METER_SERIAL_NUMBER
    ,{E_CLD_SM_ATTR_ID_METER_SERIAL_NUMBER,                  E_ZCL_AF_RD,    E_ZCL_OSTRING,  (uint32)(&((tsCLD_Mirror_SM*)(0))->sMeterSerialNumber), 0}
#endif

    /* ESP Historical Consumption set attribute ID's (D.3.2.2.5) */
#ifdef CLD_SM_MIRROR_ATTR_INSTANTANEOUS_DEMAND
    ,{E_CLD_SM_ATTR_ID_INSTANTANEOUS_DEMAND,                                     E_ZCL_AF_RD,     E_ZCL_INT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->i24InstantaneousDemand), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_DAY_CONSUMPTION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_DAY_CONSUMPTION_DELIVERED,                        E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24CurrentDayConsumptionDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_DAY_CONSUMPTION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_DAY_CONSUMPTION_RECEIVED,                         E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24CurrentDayConsumptionReceived), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_PREVIOUS_DAY_CONSUMPTION_DELIVERED
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_DAY_CONSUMPTION_DELIVERED,                       E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24PreviousDayConsumptionDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_PREVIOUS_DAY_CONSUMPTION_RECEIVED
    ,{E_CLD_SM_ATTR_ID_PREVIOUS_DAY_CONSUMPTION_RECEIVED,                        E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24PreviousDayConsumptionReceived), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED,    E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_Mirror_SM*)(0))->utctCurrentPartialProfileIntervalStartTimeDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED,     E_ZCL_AF_RD,    E_ZCL_UTCT,    (uint32)(&((tsCLD_Mirror_SM*)(0))->utctCurrentPartialProfileIntervalStartTimeReceived), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED,         E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24CurrentPartialProfileIntervalValueDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
    ,{E_CLD_SM_ATTR_ID_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED,          E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24CurrentPartialProfileIntervalValueReceived), 0}
#endif

    /* Load Profile attribute set attribute ID's (D.3.2.2.6) */
#ifdef CLD_SM_MIRROR_ATTR_MAX_NUMBER_OF_PERIODS_DELIVERED
    ,{E_CLD_SM_ATTR_ID_MAX_NUMBER_OF_PERIODS_DELIVERED,  E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8MaxNumberOfPeriodsDelivered), 0}
#endif

    /* Supply Limit attribute set attribute ID's (D.3.2.2.7) */
#ifdef CLD_SM_MIRROR_ATTR_CURRENT_DEMAND_DELIVERED
    ,{E_CLD_SM_ATTR_ID_CURRENT_DEMAND_DELIVERED,         E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24CurrentDemandDelivered), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_DEMAND_LIMIT
    ,{E_CLD_SM_ATTR_ID_DEMAND_LIMIT,                     E_ZCL_AF_RD,    E_ZCL_UINT24,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u24DemandLimit), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_DEMAND_INTEGRATION_PERIOD
    ,{E_CLD_SM_ATTR_ID_DEMAND_INTEGRATION_PERIOD,        E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8DemandIntegrationPeriod), 0}
#endif

#ifdef CLD_SM_MIRROR_ATTR_NUMBER_OF_DEMAND_SUBINTERVALS
    ,{E_CLD_SM_ATTR_ID_NUMBER_OF_DEMAND_SUBINTERVALS,    E_ZCL_AF_RD,    E_ZCL_UINT8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8NumberOfDemandSubintervals),0}
#endif

    /* Manufacturer Specific Attributes: Measurements */
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_AC_FREQUENCY
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_AC_FREQUENCY,        (E_ZCL_AF_RD | E_ZCL_AF_MS),    E_ZCL_UINT8,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u8ManSpecACFrequency), 0}
#endif
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_RMS_VOLTAGE
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_RMS_VOLTAGE,        (E_ZCL_AF_RD | E_ZCL_AF_MS),    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48ManSpecRMSVoltage), 0}
#endif
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_RMS_CURRENT
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_RMS_CURRENT,        (E_ZCL_AF_RD | E_ZCL_AF_MS),    E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48ManSpecRMSCurrent), 0}
#endif
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_ACTIVE_POWER
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_RMS_CURRENT,    (E_ZCL_AF_RD | E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48ManSpecActivePower), 0}
#endif

    /* Manufacturer Specific Attributes: Calibration,Threshold,Update Interval */
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_CALIBRATION_VPP
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_VPP,    (E_ZCL_AF_RD |  E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48ManSpecCalibrationVpp), 0}
#endif
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_CALIBRATION_I1PP
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_I1PP,   (E_ZCL_AF_RD | E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48ManSpecCalibrationI1pp), 0}
#endif
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_CALIBRATION_I2PP
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_I2PP,   (E_ZCL_AF_RD | E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48ManSpecCalibrationI2pp), 0}
#endif
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_CALIBRATION_DELTAPHI1
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_DELTAPHI1,  (E_ZCL_AF_RD | E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48ManSpecCalibrationDeltaPhi1), 0}
#endif
#ifdef CLD_SM_MIRROR_ATTR_MAN_SPEC_CALIBRATION_DELTAPHI2
    ,{E_CLD_SM_ATTR_ID_MAN_SPEC_CALIBRATION_DELTAPHI2,  (E_ZCL_AF_RD | E_ZCL_AF_MS),     E_ZCL_UINT48,    (uint32)(&((tsCLD_Mirror_SM*)(0))->u48ManSpecCalibrationDeltaPhi2), 0}
#endif

};

/*NUMBER Of Mirrors*/
tsZCL_ClusterDefinition sCLD_SM_Mirror[CLD_SM_NUMBER_OF_MIRRORS];
uint8  au8SMMirrorClusterAttributeControlBits[CLD_SM_NUMBER_OF_MIRRORS][(sizeof(asCLD_SMMirrorClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif /*CLD_SM_SUPPORT_MIRROR*/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eSE_SMCreate
 **
 ** DESCRIPTION: Configures Simple metering cluster
 **
 ** PARAMETERS:
 ** u8Endpoint,                    : Cluster attached to which EP
 ** bIsServer                    : Direction bit of a cluster
 ** *psAttributeStatus,            : Pointer to Attribute status pointer
 ** *psClusterInstance             : Cluster Instance
 ** *psClusterDefinition         : cluster definition
 ** *psCustomDataStruct            : Pointer to Custom Data structure for CallBacks
 ** *pvEndPointSharedStructPtr    : Attribute space
 **
 **
 **
 ** RETURN:
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSE_SMCreate(uint8                               u8Endpoint,
                                                bool_t                              bIsServer,
                                                uint8                              *pu8AttributeControlBits,
                                                tsZCL_ClusterInstance              *psClusterInstance,
                                                tsZCL_ClusterDefinition            *psClusterDefinition,
                                                tsSM_CustomStruct                  *psCustomDataStruct,
                                                void                               *pvEndPointSharedStructPtr)
{

    teZCL_Status eZCLstatus = E_ZCL_SUCCESS;
#if defined( CLD_SM_ATTR_SITE_ID) || defined (CLD_SM_ATTR_METER_SERIAL_NUMBER )
    tsCLD_SimpleMetering *psCLD_SimpleMetering;
#endif
#ifdef CLD_SM_SUPPORT_GET_PROFILE
#ifdef SM_SERVER
    uint8 u8LoopCntr = 0;
#endif
#endif

    #ifdef STRICT_PARAM_CHECK 
        if((psClusterInstance==NULL)            ||
           (psClusterDefinition==NULL)          ||
           (psCustomDataStruct == NULL)
           )
        {
            return E_ZCL_ERR_PARAMETER_NULL;
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
                                   eSimpleMeteringCommandHandler);     

    psClusterInstance->pvEndPointCustomStructPtr = psCustomDataStruct;
    psCustomDataStruct->sSMCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStruct->sSMCustomCallBackEvent.psClusterInstance = psClusterInstance;
    psCustomDataStruct->sSMCustomCallBackEvent.u8EndPoint = u8Endpoint;
    psCustomDataStruct->sSMCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = SE_CLUSTER_ID_SIMPLE_METERING;
    psCustomDataStruct->sSMCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&psCustomDataStruct->sSMCallBackMessage;
#if defined( CLD_SM_ATTR_SITE_ID) || defined (CLD_SM_ATTR_METER_SERIAL_NUMBER )
    //  attributes set default values
    psCLD_SimpleMetering = (tsCLD_SimpleMetering *)pvEndPointSharedStructPtr;

    /* If both SM_Server & SM_Client enabled, when creating client cluster instance NULL is passed for end point shared structure */
    if(pvEndPointSharedStructPtr)
    {
#ifdef CLD_SM_ATTR_SITE_ID
        psCLD_SimpleMetering->sSiteId.u8MaxLength = SE_SM_SITE_ID_MAX_STRING_LENGTH;
        psCLD_SimpleMetering->sSiteId.u8Length = 0;
        psCLD_SimpleMetering->sSiteId.pu8Data = psCLD_SimpleMetering->au8SiteId;
#endif

#ifdef CLD_SM_ATTR_METER_SERIAL_NUMBER
        psCLD_SimpleMetering->sMeterSerialNumber.u8MaxLength = SE_SM_METER_SERIAL_NUMBER_MAX_STRING_LENGTH;
        psCLD_SimpleMetering->sMeterSerialNumber.u8Length = 0;
        psCLD_SimpleMetering->sMeterSerialNumber.pu8Data = psCLD_SimpleMetering->au8MeterSerialNumber;
#endif

#ifdef CLD_SM_ATTR_ACTIVE_REGISTER_TIER_DELIVERED
        psCLD_SimpleMetering->sActiveRegisterTierDelivered.u8MaxLength = 4;
        psCLD_SimpleMetering->sActiveRegisterTierDelivered.u8Length = 0;
        psCLD_SimpleMetering->sActiveRegisterTierDelivered.pu8Data = psCLD_SimpleMetering->au8ActiveRegisterTierDelivered;
#endif

    }
    //psCLD_SimpleMetering->u8MeterStatus=1;



#ifdef    CLD_SM_SUPPORT_FAST_POLL_MODE

    if (bIsServer)
    {
        eZCLstatus = eSM_RegisterTimeServer();
    }

#endif

#ifdef CLD_SM_SUPPORT_GET_PROFILE


    if(bIsServer)
    {
        b8SupportedChannels = 0;
#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
        b8SupportedChannels = b8SupportedChannels | (1 << CLD_SM_PROFILE_INTERVAL_VALUE_DELIVERED);
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
        b8SupportedChannels = b8SupportedChannels | (1 << CLD_SM_PROFILE_INTERVAL_VALUE_RECEIVED);
#endif
        /* Init the end time as 0xFFFFFFFF in circular buffer */
        for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_GETPROFILE_MAX_NO_INTERVALS; u8LoopCntr++)
        {
            sSEGetProfile[u8LoopCntr].u32UtcTime = 0xFFFFFFFF;
        }

        /* Init Max. number of periods are delivered */
        if(pvEndPointSharedStructPtr)
            psCLD_SimpleMetering->u8MaxNumberOfPeriodsDelivered = CLD_SM_GETPROFILE_MAX_NO_INTERVALS;
    }
#endif

#endif
    return eZCLstatus;
}

#ifdef CLD_SM_SUPPORT_MIRROR
/****************************************************************************
 *
 * NAME: eSE_RegisterMirroredSMClusters
 *
 * DESCRIPTION:
 * Registers a simple metering cluster with the ZCL layer, for each mirror
 *
 * PARAMETERS:  Name                            Usage
 *
 * RETURNS:
 * Void
 *
 ****************************************************************************/
PUBLIC  teZCL_Status eSE_RegisterMirroredSMClusters(uint8 u8MirrorNum,
                                                                    uint8 u8MirrorStartEndPoint,
                                                                    tsZCL_ClusterInstance *psClusterInstance,
                                                                    void                  *pvEndPointSharedStructPtr,
                                                                    tsSM_CustomStruct     *psCustomDataStruct)
{

    sCLD_SM_Mirror[u8MirrorNum].u16ClusterEnum = SE_CLUSTER_ID_SIMPLE_METERING;
    sCLD_SM_Mirror[u8MirrorNum].bIsManufacturerSpecificCluster = FALSE;
    sCLD_SM_Mirror[u8MirrorNum].u8ClusterControlFlags = (E_ZCL_SECURITY_NETWORK | CLUSTER_MIRROR_BIT);
    sCLD_SM_Mirror[u8MirrorNum].u16NumberOfAttributes = (sizeof(asCLD_SMMirrorClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition));
    sCLD_SM_Mirror[u8MirrorNum].psAttributeDefinition =  (tsZCL_AttributeDefinition*)asCLD_SMMirrorClusterAttributeDefinitions;

    psClusterInstance->bIsServer = TRUE;
    psClusterInstance->psClusterDefinition = &sCLD_SM_Mirror[u8MirrorNum];
    psClusterInstance->pvEndPointSharedStructPtr = pvEndPointSharedStructPtr;
    psClusterInstance->pu8AttributeControlBits = au8SMMirrorClusterAttributeControlBits[u8MirrorNum];
    psClusterInstance->pCustomcallCallBackFunction = eSimpleMeteringCommandHandler;
    psClusterInstance->pvEndPointCustomStructPtr = psCustomDataStruct;
    psCustomDataStruct->sSMCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStruct->sSMCustomCallBackEvent.psClusterInstance = psClusterInstance;
    psCustomDataStruct->sSMCustomCallBackEvent.u8EndPoint = u8MirrorStartEndPoint + u8MirrorNum;
    psCustomDataStruct->sSMCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = SE_CLUSTER_ID_SIMPLE_METERING;
    psCustomDataStruct->sSMCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&psCustomDataStruct->sSMCallBackMessage;
#ifdef CLD_SM_MIRROR_ATTR_METER_SERIAL_NUMBER
    ((tsCLD_Mirror_SM *)(pvEndPointSharedStructPtr))->sMeterSerialNumber.u8MaxLength = SE_SM_METER_SERIAL_NUMBER_MAX_STRING_LENGTH;
    ((tsCLD_Mirror_SM *)(pvEndPointSharedStructPtr))->sMeterSerialNumber.u8Length = 0;
    ((tsCLD_Mirror_SM *)(pvEndPointSharedStructPtr))->sMeterSerialNumber.pu8Data =
        ((tsCLD_Mirror_SM *)(pvEndPointSharedStructPtr))->au8MeterSerialNumber;
#endif



    return E_ZCL_SUCCESS;
}
#endif /*CLD_SM_SUPPORT_MIRROR*/


/****************************************************************************
 **
 ** NAME:       eSE_FindSimpleMeteringCluster
 **
 ** DESCRIPTION:
 ** Search helper function
 **
 ** PARAMETERS:                     Name                        Usage
 ** uint8                           u8SourceEndPointId          Source EP Id
 ** bool_t                          bIsServer                   Is server
 ** tsZCL_EndPointDefinition    **ppsEndPointDefinition         EP
 ** tsZCL_ClusterInstance       **ppsClusterInstance            Cluster
 ** tsSE_DRLCCustomDataStructure  **ppsDRLCCustomDataStructure  event data
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eSE_FindSimpleMeteringCluster(
                                uint8                           u8SourceEndPointId,
                                bool_t                          bIsServer,
                                tsZCL_EndPointDefinition      **ppsEndPointDefinition,
                                tsZCL_ClusterInstance         **ppsClusterInstance,
                                tsSM_CustomStruct                **ppsSM_CustomStruct)
{
    // check EP is registered and cluster is present in the send device
    if(eZCL_SearchForEPentry(u8SourceEndPointId, ppsEndPointDefinition) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    if(eZCL_SearchForClusterEntry(u8SourceEndPointId, SE_CLUSTER_ID_SIMPLE_METERING, bIsServer, ppsClusterInstance) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
    }

    // initialise custom data pointer
    *ppsSM_CustomStruct = (tsSM_CustomStruct *)(*ppsClusterInstance)->pvEndPointCustomStructPtr;
    if(*ppsSM_CustomStruct == NULL)
    {
        return(E_ZCL_ERR_CUSTOM_DATA_NULL);
    }

    return(E_ZCL_SUCCESS);
}




#endif  /*CLD_SIMPLE_METERING*/

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

