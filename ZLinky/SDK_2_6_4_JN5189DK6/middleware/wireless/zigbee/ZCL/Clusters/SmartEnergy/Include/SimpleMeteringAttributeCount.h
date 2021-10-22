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
 * COMPONENT:          SimpleMeteringAttributeCount.h
 *
 * DESCRIPTION:        Header for finding the total number of Simple Metering attributes
 *
 *****************************************************************************/

#ifndef SIMPLE_METERING_ATTRIBUTE_COUNT_H
#define SIMPLE_METERING_ATTRIBUTE_COUNT_H

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define CLD_SM_NUMBER_OF_MANDATORY_ATTRIBUTE                        (5) //u48CurrentSummationDelivered, eUnitOfMeasure, u8MeterStatus, u8SummationFormatting,eMeteringDeviceType

#ifdef CLD_SM_ATTR_CURRENT_SUMMATION_RECEIVED
   #define ATTR_ID_CURRENT_SUMMATION_RECEIVED                       (1)
#else
    #define ATTR_ID_CURRENT_SUMMATION_RECEIVED                      (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_MAX_DEMAND_DELIVERED
    #define ATTR_ID_CURRENT_MAX_DEMAND_DELIVERED                    (1)
#else
    #define ATTR_ID_CURRENT_MAX_DEMAND_DELIVERED                    (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_MAX_DEMAND_RECEIVED
    #define ATTR_ID_CURRENT_MAX_DEMAND_RECEIVED                     (1)
#else
    #define ATTR_ID_CURRENT_MAX_DEMAND_RECEIVED                     (0)
#endif


#ifdef CLD_SM_ATTR_DFT_SUMMATION
    #define ATTR_DFT_SUMMATION                                      (1)
#else
    #define ATTR_DFT_SUMMATION                                      (0)
#endif


#ifdef CLD_SM_ATTR_DAILY_FREEZE_TIME
    #define ATTR_DAILY_FREEZE_TIME                                  (1)
#else
    #define ATTR_DAILY_FREEZE_TIME                                  (0)
#endif


#ifdef CLD_SM_ATTR_POWER_FACTOR
    #define ATTR_POWER_FACTOR                                       (1)
#else
    #define ATTR_POWER_FACTOR                                       (0)
#endif


#ifdef CLD_SM_ATTR_READING_SNAPSHOT_TIME
    #define ATTR_READING_SNAPSHOT_TIME                              (1)
#else
    #define ATTR_READING_SNAPSHOT_TIME                              (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_MAX_DEMAND_DELIVERED_TIME
    #define ATTR_CURRENT_MAX_DEMAND_DELIVERED_TIME                  (1)
#else
    #define ATTR_CURRENT_MAX_DEMAND_DELIVERED_TIME                  (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_MAX_DEMAND_RECEIVED_TIME
    #define ATTR_CURRENT_MAX_DEMAND_RECEIVED_TIME                   (1)
#else
    #define ATTR_CURRENT_MAX_DEMAND_RECEIVED_TIME                   (0)
#endif


#ifdef CLD_SM_ATTR_DEFAULT_UPDATE_PERIOD
    #define ATTR_DEFAULT_UPDATE_PERIOD                              (1)
#else
    #define ATTR_DEFAULT_UPDATE_PERIOD                              (0)
#endif


#ifdef CLD_SM_ATTR_FAST_POLL_UPDATE_PERIOD
    #define ATTR_FAST_POLL_UPDATE_PERIOD                            (1)
#else
    #define ATTR_FAST_POLL_UPDATE_PERIOD                            (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED
    #define ATTR_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED         (1)
#else
    #define ATTR_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED         (0)
#endif


#ifdef CLD_SM_ATTR_DAILY_CONSUMPTION_TARGET
    #define ATTR_DAILY_CONSUMPTION_TARGET                           (1)
#else
    #define ATTR_DAILY_CONSUMPTION_TARGET                           (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_BLOCK
    #define ATTR_CURRENT_BLOCK                                      (1)
#else
    #define ATTR_CURRENT_BLOCK                                      (0)
#endif


#ifdef CLD_SM_SUPPORT_GET_PROFILE
    #ifdef CLD_SM_ATTR_PROFILE_INTERVAL_PERIOD
        #define ATTR_PROFILE_INTERVAL_PERIOD                        (1)
    #else
        #define ATTR_PROFILE_INTERVAL_PERIOD                        (0)
    #endif
    #ifdef CLD_SM_ATTR_INTERVAL_READ_REPORTING_PERIOD
        #define ATTR_INTERVAL_READ_REPORTING_PERIOD                 (1)
    #else
        #define ATTR_INTERVAL_READ_REPORTING_PERIOD                 (0)
    #endif

#else
   #define ATTR_INTERVAL_READ_REPORTING_PERIOD                      (0)
   #define ATTR_PROFILE_INTERVAL_PERIOD                             (0)
#endif // CLD_SM_SUPPORT_GET_PROFILE

#ifdef CLD_SM_ATTR_PRESET_READING_TIME
    #define ATTR_PRESET_READING_TIME                                (1)
#else
    #define ATTR_PRESET_READING_TIME                                (0)
#endif

#ifdef CLD_SM_ATTR_SUMMATION_DELIVERED_PER_REPORT
    #define ATTR_SUMMATION_DELIVERED_PER_REPORT                     (1)
#else
    #define ATTR_SUMMATION_DELIVERED_PER_REPORT                     (0)
#endif

#ifdef CLD_SM_ATTR_FLOW_RESTRICTION
    #define ATTR_FLOW_RESTRICTION                                   (1)
#else
    #define ATTR_FLOW_RESTRICTION                                   (0)
#endif

#ifdef CLD_SM_ATTR_SUPPLY_STATUS
    #define ATTR_SUPPLY_STATUS                                      (1)
#else
    #define ATTR_SUPPLY_STATUS                                      (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_INLET_ENERGY_CARRIER_SUMMATION
    #define ATTR_CURRENT_INLET_ENERGY_CARRIER_SUMMATION             (1)
#else
    #define ATTR_CURRENT_INLET_ENERGY_CARRIER_SUMMATION             (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_OUTLET_ENERGY_CARRIER_SUMMATION
    #define ATTR_CURRENT_OUTLET_ENERGY_CARRIER_SUMMATION            (1)
#else
    #define ATTR_CURRENT_OUTLET_ENERGY_CARRIER_SUMMATION            (0)
#endif

#ifdef CLD_SM_ATTR_INLET_TEMPERATURE
    #define ATTR_INLET_TEMPERATURE                                  (1)
#else
    #define ATTR_INLET_TEMPERATURE                                  (0)
#endif

#ifdef CLD_SM_ATTR_OUTLET_TEMPERATURE
    #define ATTR_OUTLET_TEMPERATURE                                 (1)
#else
    #define ATTR_OUTLET_TEMPERATURE                                 (0)
#endif

#ifdef CLD_SM_ATTR_CONTROL_TEMPERATURE
    #define ATTR_CONTROL_TEMPERATURE                                (1)
#else
    #define ATTR_CONTROL_TEMPERATURE                                (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_INLET_ENERGY_CARRIER_DEMAND
    #define ATTR_CURRENT_INLET_ENERGY_CARRIER_DEMAND                (1)
#else
    #define ATTR_CURRENT_INLET_ENERGY_CARRIER_DEMAND                (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_OUTLET_ENERGY_CARRIER_DEMAND
    #define ATTR_CURRENT_OUTLET_ENERGY_CARRIER_DEMAND               (1)
#else
    #define ATTR_CURRENT_OUTLET_ENERGY_CARRIER_DEMAND               (0)
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_BLOCK_PERIOD_CONSUMPTION_DELIVERED
    #define ATTR_PREVIOUS_BLOCK_PERIOD_CONSUMPTION_DELIVERED        (1)
#else
    #define ATTR_PREVIOUS_BLOCK_PERIOD_CONSUMPTION_DELIVERED        (0)
#endif

/* Time Of Use Information attribute attribute ID's set (D.3.2.2.2) */
#ifdef CLD_SM_ATTR_CURRENT_TIER_1_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_1_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_1_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_1_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_1_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_1_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_2_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_2_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_2_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_2_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_2_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_2_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_3_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_3_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_3_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_3_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_3_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_3_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_4_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_4_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_4_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_4_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_4_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_4_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_5_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_5_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_5_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_5_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_5_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_5_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_6_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_6_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_6_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_6_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_6_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_6_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_7_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_7_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_7_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_7_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_7_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_7_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_8_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_8_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_8_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_8_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_8_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_8_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_9_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_9_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_9_SUMMATION_DELIVERED                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_9_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_9_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_9_SUMMATION_RECEIVED                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_10_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_10_SUMMATION_DELIVERED                (1)
#else
    #define ATTR_CURRENT_TIER_10_SUMMATION_DELIVERED                (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_TIER_10_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_10_SUMMATION_RECEIVED                 (1)
#else
    #define ATTR_CURRENT_TIER_10_SUMMATION_RECEIVED                 (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_11_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_11_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_11_SUMMATION_DELIVERED                 (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_11_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_11_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_11_SUMMATION_RECEIVED                  (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_12_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_12_SUMMATION_DELIVERED                 (1)
#else
    #define ATTR_CURRENT_TIER_12_SUMMATION_DELIVERED                 (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_12_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_12_SUMMATION_RECEIVED                  (1)
#else
    #define ATTR_CURRENT_TIER_12_SUMMATION_RECEIVED                  (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_13_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_13_SUMMATION_DELIVERED                          (1)
#else
    #define ATTR_CURRENT_TIER_13_SUMMATION_DELIVERED                          (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_13_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_13_SUMMATION_RECEIVED                           (1)
#else
    #define ATTR_CURRENT_TIER_13_SUMMATION_RECEIVED                           (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_14_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_14_SUMMATION_DELIVERED                          (1)
#else
    #define ATTR_CURRENT_TIER_14_SUMMATION_DELIVERED                          (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_14_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_14_SUMMATION_RECEIVED                           (1)
#else
    #define ATTR_CURRENT_TIER_14_SUMMATION_RECEIVED                           (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_15_SUMMATION_DELIVERED
    #define ATTR_CURRENT_TIER_15_SUMMATION_DELIVERED                          (1)
#else
    #define ATTR_CURRENT_TIER_15_SUMMATION_DELIVERED                          (0)
#endif


#ifdef CLD_SM_ATTR_CURRENT_TIER_15_SUMMATION_RECEIVED
    #define ATTR_CURRENT_TIER_15_SUMMATION_RECEIVED                           (1)
#else
    #define ATTR_CURRENT_TIER_15_SUMMATION_RECEIVED                           (0)
#endif



#ifdef CLD_SM_ATTR_REMAINING_BATTERY_LIFE
    #define ATTR_REMAINING_BATTERY_LIFE                                       (1)
#else
    #define ATTR_REMAINING_BATTERY_LIFE                                       (0)
#endif


#ifdef CLD_SM_ATTR_HOURS_IN_OPERATION
   #define ATTR_HOURS_IN_OPERATION                                            (1)
#else
    #define ATTR_HOURS_IN_OPERATION                                           (0)
#endif


#ifdef CLD_SM_ATTR_HOURS_IN_FAULT
    #define ATTR_HOURS_IN_FAULT                                               (1)
#else
    #define ATTR_HOURS_IN_FAULT                                               (0)
#endif



#ifdef CLD_SM_ATTR_MULTIPLIER
    #define ATTR_MULTIPLIER                                                   (1)
#else
    #define ATTR_MULTIPLIER                                                   (0)
#endif

#ifdef CLD_SM_ATTR_DIVISOR
    #define ATTR_DIVISOR                                                      (1)
#else
    #define ATTR_DIVISOR                                                      (0)
#endif

#ifdef CLD_SM_ATTR_DEMAND_FORMATING
    #define ATTR_DEMAND_FORMATING                                             (1)
#else
    #define ATTR_DEMAND_FORMATING                                             (0)
#endif

#ifdef CLD_SM_ATTR_HISTORICAL_CONSUMPTION_FORMATTING
    #define ATTR_HISTORICAL_CONSUMPTION_FORMATTING                            (1)
#else
    #define ATTR_HISTORICAL_CONSUMPTION_FORMATTING                            (0)
#endif

#ifdef CLD_SM_ATTR_SITE_ID
    #define ATTR_SITE_ID                                                      (1)
#else
    #define ATTR_SITE_ID                                                      (0)
#endif

#ifdef CLD_SM_ATTR_METER_SERIAL_NUMBER
    #define ATTR_METER_SERIAL_NUMBER                                          (1)
#else
    #define ATTR_METER_SERIAL_NUMBER                                          (0)
#endif

#ifdef CLD_SM_ATTR_ENERGY_CARRIER_UNIT_OF_MEASURE
    #define ATTR_ENERGY_CARRIER_UNIT_OF_MEASURE                               (1)
#else
    #define ATTR_ENERGY_CARRIER_UNIT_OF_MEASURE                               (0)
#endif

#ifdef CLD_SM_ATTR_ENERGY_CARRIER_SUMMATION_FORMATTING
    #define ATTR_ENERGY_CARRIER_SUMMATION_FORMATTING                          (1)
#else
    #define ATTR_ENERGY_CARRIER_SUMMATION_FORMATTING                          (0)
#endif

#ifdef CLD_SM_ATTR_ENERGY_CARRIER_DEMAND_FORMATTING
    #define ATTR_ENERGY_CARRIER_DEMAND_FORMATTING                             (1)
#else
    #define ATTR_ENERGY_CARRIER_DEMAND_FORMATTING                             (0)
#endif

#ifdef CLD_SM_ATTR_TEMPERATURE_UNIT_OF_MEASURE
    #define ATTR_TEMPERATURE_UNIT_OF_MEASURE                                  (1)
#else
    #define ATTR_TEMPERATURE_UNIT_OF_MEASURE                                  (0)
#endif

#ifdef CLD_SM_ATTR_TEMPERATURE_FORMATTING
    #define ATTR_TEMPERATURE_FORMATTING                                       (1)
#else
    #define ATTR_TEMPERATURE_FORMATTING                                       (0)
#endif
 /* ESP Historical Consumption set attribute ID's (D.3.2.2.5) */
#ifdef CLD_SM_ATTR_INSTANTANEOUS_DEMAND
    #define ATTR_ID_INSTANTANEOUS_DEMAND                                      (1)
#else
    #define ATTR_ID_INSTANTANEOUS_DEMAND                                      (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_CONSUMPTION_DELIVERED
    #define ATTR_CURRENT_DAY_CONSUMPTION_DELIVERED                            (1)
#else
    #define ATTR_CURRENT_DAY_CONSUMPTION_DELIVERED                            (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_CONSUMPTION_RECEIVED
    #define ATTR_CURRENT_DAY_CONSUMPTION_RECEIVED                             (1)
#else
    #define ATTR_CURRENT_DAY_CONSUMPTION_RECEIVED                             (0)
#endif


#ifdef CLD_SM_ATTR_PREVIOUS_DAY_CONSUMPTION_DELIVERED
    #define ATTR_PREVIOUS_DAY_CONSUMPTION_DELIVERED                           (1)
#else
    #define ATTR_PREVIOUS_DAY_CONSUMPTION_DELIVERED                           (0)
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_CONSUMPTION_RECEIVED
    #define ATTR_PREVIOUS_DAY_CONSUMPTION_RECEIVED                            (1)
#else
    #define ATTR_PREVIOUS_DAY_CONSUMPTION_RECEIVED                            (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED
   #define ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED         (1)
#else
    #define ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED        (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED
   #define ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED          (1)
#else
    #define ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED         (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
    #define ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED             (1)
#else
    #define ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED             (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
    #define ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED              (1)
#else
    #define ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED              (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_MAXIMUM_PRESSURE
    #define ATTR_CURRENT_DAY_MAXIMUM_PRESSURE                                 (1)
#else
    #define ATTR_CURRENT_DAY_MAXIMUM_PRESSURE                                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_MINIMUM_PRESSURE
   #define ATTR_CURRENT_DAY_MINIMUM_PRESSURE                                  (1)
#else
    #define ATTR_CURRENT_DAY_MINIMUM_PRESSURE                                 (0)
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_MAXIMUM_PRESSURE
    #define ATTR_PREVIOUS_DAY_MAXIMUM_PRESSURE                                (1)
#else
    #define ATTR_PREVIOUS_DAY_MAXIMUM_PRESSURE                                (0)
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_MINIMUM_PRESSURE
    #define ATTR_PREVIOUS_DAY_MINIMUM_PRESSURE                                (1)
#else
    #define ATTR_PREVIOUS_DAY_MINIMUM_PRESSURE                                (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_MAXIMUM_DEMAND
   #define ATTR_CURRENT_DAY_MAXIMUM_DEMAND                                    (1)
#else
    #define ATTR_CURRENT_DAY_MAXIMUM_DEMAND                                   (0)
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_MAXIMUM_DEMAND
    #define ATTR_PREVIOUS_DAY_MAXIMUM_DEMAND                                  (1)
#else
    #define ATTR_PREVIOUS_DAY_MAXIMUM_DEMAND                                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_MONTH_MAXIMUM_DEMAND
    #define ATTR_CURRENT_MONTH_MAXIMUM_DEMAND                                 (1)
#else
    #define ATTR_CURRENT_MONTH_MAXIMUM_DEMAND                                 (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_YEAR_MAXIMUM_DEMAND
    #define ATTR_CURRENT_YEAR_MAXIMUM_DEMAND                                  (1)
#else
    #define ATTR_CURRENT_YEAR_MAXIMUM_DEMAND                                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND
 #define ATTR_CURRENT_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND                       (1)
#else
    #define ATTR_CURRENT_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND                    (0)
#endif

#ifdef CLD_SM_ATTR_PREVIOUS_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND
 #define ATTR_PREVIOUS_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND                      (1)
#else
    #define ATTR_PREVIOUS_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND                   (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_MONTH_MAXIMUM_ENERGY_CARRIER_DEMAND
    #define ATTR_CURRENT_MONTH_MAXIMUM_ENERGY_CARRIER_DEMAND                  (1)
#else
    #define ATTR_CURRENT_MONTH_MAXIMUM_ENERGY_CARRIER_DEMAND                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_MONTH_MINIMUM_ENERGY_CARRIER_DEMAND
    #define ATTR_CURRENT_MONTH_MINIMUM_ENERGY_CARRIER_DEMAND                  (1)
#else
    #define ATTR_CURRENT_MONTH_MINIMUM_ENERGY_CARRIER_DEMAND                  (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_YEAR_MAXIMUM_ENERGY_CARRIER_DEMAND
    #define ATTR_CURRENT_YEAR_MAXIMUM_ENERGY_CARRIER_DEMAND                   (1)
#else
    #define ATTR_CURRENT_YEAR_MAXIMUM_ENERGY_CARRIER_DEMAND                   (0)
#endif

#ifdef CLD_SM_ATTR_CURRENT_YEAR_MINIMUM_ENERGY_CARRIER_DEMAND
    #define ATTR_CURRENT_YEAR_MINIMUM_ENERGY_CARRIER_DEMAND                   (1)
#else
    #define ATTR_CURRENT_YEAR_MINIMUM_ENERGY_CARRIER_DEMAND                   (0)
#endif

/* Load Profile attribute set attribute ID's (D.3.2.2.6) */
#ifdef CLD_SM_ATTR_MAX_NUMBER_OF_PERIODS_DELIVERED
    #define ATTR_MAX_NUMBER_OF_PERIODS_DELIVERED                              (1)
#else
    #define ATTR_MAX_NUMBER_OF_PERIODS_DELIVERED                              (0)
#endif

/* Supply Limit attribute set attribute ID's (D.3.2.2.7) */
#ifdef CLD_SM_ATTR_CURRENT_DEMAND_DELIVERED
    #define ATTR_CURRENT_DEMAND_DELIVERED                                     (1)
#else
    #define ATTR_CURRENT_DEMAND_DELIVERED                                     (0)
#endif

#ifdef CLD_SM_ATTR_DEMAND_LIMIT
    #define ATTR_DEMAND_LIMIT                                                 (1)
#else
    #define ATTR_DEMAND_LIMIT                                                 (0)
#endif

#ifdef CLD_SM_ATTR_DEMAND_INTEGRATION_PERIOD
    #define ATTR_DEMAND_INTEGRATION_PERIOD                                    (1)
#else
    #define ATTR_DEMAND_INTEGRATION_PERIOD                                    (0)
#endif

#ifdef CLD_SM_ATTR_NUMBER_OF_DEMAND_SUBINTERVALS
    #define ATTR_NUMBER_OF_DEMAND_SUBINTERVALS                                (1)
#else
    #define ATTR_NUMBER_OF_DEMAND_SUBINTERVALS                                (0)
#endif

    /* Block Information attribute set attribute ID's (D.3.2.2.8) */
    /* No Tier Block Set */
#if (CLD_SM_ATTR_NO_TIER_BLOCK_CURRENT_SUMMATION_DELIVERED_MAX_COUNT != 0)
    #define ATTR_ID_CURRENT_NOTIER_BLOCK1_SUMMATION_DELIVERED                 \
    CLD_SM_ATTR_NO_TIER_BLOCK_CURRENT_SUMMATION_DELIVERED_MAX_COUNT
#else
    #define ATTR_ID_CURRENT_NOTIER_BLOCK1_SUMMATION_DELIVERED                 (0)
#endif

/* Tier1 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 0)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER1_BLOCK1_SUMMATION_DELIVERED                   \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER1_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif

    /* Tier2 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 1)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER2_BLOCK1_SUMMATION_DELIVERED                   \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER2_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif

 /* Tier3 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 2)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
 #define ATTR_ID_CURRENT_TIER3_BLOCK1_SUMMATION_DELIVERED                      \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER3_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif

/* Tier4 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 3)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
   #define ATTR_ID_CURRENT_TIER4_BLOCK1_SUMMATION_DELIVERED                      \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER4_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif

 /* Tier5 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 4)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER5_BLOCK1_SUMMATION_DELIVERED                      \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER5_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif

/* Tier6 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 5)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
   #define ATTR_ID_CURRENT_TIER6_BLOCK1_SUMMATION_DELIVERED                    \
          (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER6_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif

/* Tier7 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 6)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER7_BLOCK1_SUMMATION_DELIVERED                   \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER7_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif

/* Tier8 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 7)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER8_BLOCK1_SUMMATION_DELIVERED                      \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER8_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif

/* Tier9 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 8)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER9_BLOCK1_SUMMATION_DELIVERED                      \
           (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER9_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif


    /* Tier10 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 9)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER10_BLOCK1_SUMMATION_DELIVERED                     \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER10_BLOCK1_SUMMATION_DELIVERED                 (0)
#endif


    /* Tier11 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 10)&&             \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER11_BLOCK1_SUMMATION_DELIVERED                     \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER11_BLOCK1_SUMMATION_DELIVERED                 (0)
#endif


    /* Tier12 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 11)&&             \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
    #define ATTR_ID_CURRENT_TIER12_BLOCK1_SUMMATION_DELIVERED                 \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER12_BLOCK1_SUMMATION_DELIVERED                 (0)
#endif


    /* Tier13 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 12)&&             \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
 #define ATTR_ID_CURRENT_TIER13_BLOCK1_SUMMATION_DELIVERED                     \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER13_BLOCK1_SUMMATION_DELIVERED                 (0)
#endif


    /* Tier14 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 13)&&             \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
 #define ATTR_ID_CURRENT_TIER14_BLOCK1_SUMMATION_DELIVERED                      \
                (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER14_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif


    /* Tier15 Block Set */
#if ((CLD_SM_ATTR_NUM_OF_TIERS_CURRENT_SUMMATION_DELIVERED > 14)&&              \
(CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED != 0))
 #define ATTR_ID_CURRENT_TIER15_BLOCK1_SUMMATION_DELIVERED                      \
                 (CLD_SM_ATTR_NUM_OF_BLOCKS_IN_EACH_TIER_CURRENT_SUMMATION_DELIVERED)
#else
    #define ATTR_ID_CURRENT_TIER15_BLOCK1_SUMMATION_DELIVERED                  (0)
#endif


    /* Alarm attribute set attribute ID's (D.3.2.2.9) */
#ifdef CLD_SM_ATTR_GENERIC_ALARM_MASK
 #define ATTR_ID_GENERIC_ALARM_MASK                                           (1)
#else
    #define ATTR_ID_GENERIC_ALARM_MASK                                        (0)
#endif


#ifdef CLD_SM_ATTR_ELECTRICITY_ALARM_MASK
 #define ATTR_ELECTRICITY_ALARM_MASK                                          (1)
#else
    #define ATTR_ELECTRICITY_ALARM_MASK                                       (0)
#endif


#ifdef CLD_SM_ATTR_PRESSURE_ALARM_MASK
 #define ATTR_PRESSURE_ALARM_MASK                                             (1)
#else
    #define ATTR_PRESSURE_ALARM_MASK                                          (0)
#endif

#ifdef CLD_SM_ATTR_WATER_SPECIFIC_ALARM_MASK
 #define ATTR_WATER_SPECIFIC_ALARM_MASK                                       (1)
#else
    #define ATTR_WATER_SPECIFIC_ALARM_MASK                                    (0)
#endif

#ifdef CLD_SM_ATTR_HEAT_AND_COOLING_ALARM_MASK
 #define ATTR_HEAT_AND_COOLING_ALARM_MASK                                     (1)
#else
    #define ATTR_HEAT_AND_COOLING_ALARM_MASK                                  (0)
#endif

#ifdef CLD_SM_ATTR_GAS_ALARM_MASK
 #define ATTR_GAS_ALARM_MASK                                                  (1)
#else
    #define ATTR_GAS_ALARM_MASK                                               (0)
#endif

#define CLD_SM_NUMBER_OF_OPTIONAL_ATTRIBUTE                         \
    ((ATTR_ID_CURRENT_SUMMATION_RECEIVED)                         +  \
    (ATTR_ID_CURRENT_MAX_DEMAND_DELIVERED)                        +  \
    (ATTR_ID_CURRENT_MAX_DEMAND_RECEIVED)                         +  \
    (ATTR_DFT_SUMMATION)                                          +  \
    (ATTR_DAILY_FREEZE_TIME)                                      +  \
    (ATTR_POWER_FACTOR)                                           +  \
    (ATTR_READING_SNAPSHOT_TIME)                                  +  \
    (ATTR_CURRENT_MAX_DEMAND_DELIVERED_TIME)                      +  \
    (ATTR_CURRENT_MAX_DEMAND_RECEIVED_TIME)                       +  \
    (ATTR_DEFAULT_UPDATE_PERIOD)                                  +  \
    (ATTR_FAST_POLL_UPDATE_PERIOD)                                +  \
    (ATTR_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED)             +  \
    (ATTR_DAILY_CONSUMPTION_TARGET)                               +  \
    (ATTR_CURRENT_BLOCK)                                          +  \
    (ATTR_PROFILE_INTERVAL_PERIOD)                                +  \
    (ATTR_INTERVAL_READ_REPORTING_PERIOD)                         +  \
    (ATTR_PRESET_READING_TIME)                                    +  \
    (ATTR_SUMMATION_DELIVERED_PER_REPORT)                         +  \
    (ATTR_FLOW_RESTRICTION)                                       +  \
    (ATTR_SUPPLY_STATUS)                                          +  \
    (ATTR_CURRENT_INLET_ENERGY_CARRIER_SUMMATION)                 +  \
    (ATTR_CURRENT_OUTLET_ENERGY_CARRIER_SUMMATION)                +  \
    (ATTR_INLET_TEMPERATURE)                                      +  \
    (ATTR_OUTLET_TEMPERATURE)                                     +  \
    (ATTR_CONTROL_TEMPERATURE)                                    +  \
    (ATTR_CURRENT_INLET_ENERGY_CARRIER_DEMAND)                    +  \
    (ATTR_CURRENT_OUTLET_ENERGY_CARRIER_DEMAND)                   +  \
    (ATTR_PREVIOUS_BLOCK_PERIOD_CONSUMPTION_DELIVERED)            +  \
    (ATTR_CURRENT_TIER_1_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_1_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_2_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_2_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_3_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_3_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_4_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_4_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_5_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_5_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_6_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_6_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_7_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_7_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_8_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_8_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_9_SUMMATION_DELIVERED)                     +  \
    (ATTR_CURRENT_TIER_9_SUMMATION_RECEIVED)                      +  \
    (ATTR_CURRENT_TIER_10_SUMMATION_DELIVERED)                    +  \
    (ATTR_CURRENT_TIER_10_SUMMATION_RECEIVED)                     +  \
    (ATTR_CURRENT_TIER_11_SUMMATION_DELIVERED)                    +  \
    (ATTR_CURRENT_TIER_11_SUMMATION_RECEIVED)                     +  \
    (ATTR_CURRENT_TIER_12_SUMMATION_DELIVERED)                    +  \
    (ATTR_CURRENT_TIER_12_SUMMATION_RECEIVED)                     +  \
    (ATTR_CURRENT_TIER_13_SUMMATION_DELIVERED)                    +  \
    (ATTR_CURRENT_TIER_13_SUMMATION_RECEIVED)                     +  \
    (ATTR_CURRENT_TIER_14_SUMMATION_DELIVERED)                    +  \
    (ATTR_CURRENT_TIER_14_SUMMATION_RECEIVED)                     +  \
    (ATTR_CURRENT_TIER_15_SUMMATION_DELIVERED)                    +  \
    (ATTR_CURRENT_TIER_15_SUMMATION_RECEIVED)                     +  \
    (ATTR_REMAINING_BATTERY_LIFE)                                 +  \
    (ATTR_HOURS_IN_OPERATION)                                     +  \
    (ATTR_HOURS_IN_FAULT)                                         +  \
    (ATTR_MULTIPLIER)                                             +  \
    (ATTR_DIVISOR)                                                +  \
    (ATTR_DEMAND_FORMATING)                                       +  \
    (ATTR_HISTORICAL_CONSUMPTION_FORMATTING)                      +  \
    (ATTR_SITE_ID)                                                +  \
    (ATTR_METER_SERIAL_NUMBER)                                    +  \
    (ATTR_ENERGY_CARRIER_UNIT_OF_MEASURE)                         +  \
    (ATTR_ENERGY_CARRIER_SUMMATION_FORMATTING)                    +  \
    (ATTR_ENERGY_CARRIER_DEMAND_FORMATTING)                       +  \
    (ATTR_TEMPERATURE_UNIT_OF_MEASURE)                            +  \
    (ATTR_TEMPERATURE_FORMATTING)                                 +  \
    (ATTR_ID_INSTANTANEOUS_DEMAND)                                +  \
    (ATTR_CURRENT_DAY_CONSUMPTION_DELIVERED)                      +  \
    (ATTR_CURRENT_DAY_CONSUMPTION_RECEIVED)                       +  \
    (ATTR_PREVIOUS_DAY_CONSUMPTION_DELIVERED)                     +  \
    (ATTR_PREVIOUS_DAY_CONSUMPTION_RECEIVED)                      +  \
    (ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED)  +  \
    (ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED)   +  \
    (ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED)       +  \
    (ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED)        +  \
    (ATTR_CURRENT_DAY_MAXIMUM_PRESSURE)                           +  \
    (ATTR_CURRENT_DAY_MINIMUM_PRESSURE)                           +  \
    (ATTR_PREVIOUS_DAY_MAXIMUM_PRESSURE)                          +  \
    (ATTR_PREVIOUS_DAY_MINIMUM_PRESSURE)                          +  \
    (ATTR_CURRENT_DAY_MAXIMUM_DEMAND)                             +  \
    (ATTR_PREVIOUS_DAY_MAXIMUM_DEMAND)                            +  \
    (ATTR_CURRENT_MONTH_MAXIMUM_DEMAND)                           +  \
    (ATTR_CURRENT_YEAR_MAXIMUM_DEMAND)                            +  \
    (ATTR_CURRENT_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND)              +  \
    (ATTR_PREVIOUS_DAY_MAXIMUM_ENERGY_CARRIER_DEMAND)             +  \
    (ATTR_CURRENT_MONTH_MAXIMUM_ENERGY_CARRIER_DEMAND)            +  \
    (ATTR_CURRENT_MONTH_MINIMUM_ENERGY_CARRIER_DEMAND)            +  \
    (ATTR_CURRENT_YEAR_MAXIMUM_ENERGY_CARRIER_DEMAND)             +  \
    (ATTR_CURRENT_YEAR_MINIMUM_ENERGY_CARRIER_DEMAND)             +  \
    (ATTR_MAX_NUMBER_OF_PERIODS_DELIVERED)                        +  \
    (ATTR_CURRENT_DEMAND_DELIVERED)                               +  \
    (ATTR_DEMAND_LIMIT)                                           +  \
    (ATTR_DEMAND_INTEGRATION_PERIOD)                              +  \
    (ATTR_NUMBER_OF_DEMAND_SUBINTERVALS)                          +  \
    (ATTR_ID_CURRENT_NOTIER_BLOCK1_SUMMATION_DELIVERED)           +  \
    (ATTR_ID_CURRENT_TIER1_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER2_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER3_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER4_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER5_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER6_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER7_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER8_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER9_BLOCK1_SUMMATION_DELIVERED)            +  \
    (ATTR_ID_CURRENT_TIER10_BLOCK1_SUMMATION_DELIVERED)           +  \
    (ATTR_ID_CURRENT_TIER11_BLOCK1_SUMMATION_DELIVERED)           +  \
    (ATTR_ID_CURRENT_TIER12_BLOCK1_SUMMATION_DELIVERED)           +  \
    (ATTR_ID_CURRENT_TIER13_BLOCK1_SUMMATION_DELIVERED)           +  \
    (ATTR_ID_CURRENT_TIER14_BLOCK1_SUMMATION_DELIVERED)           +  \
    (ATTR_ID_CURRENT_TIER15_BLOCK1_SUMMATION_DELIVERED)           +  \
    (ATTR_ID_GENERIC_ALARM_MASK)                                  +  \
    (ATTR_ELECTRICITY_ALARM_MASK)                                 +  \
    (ATTR_PRESSURE_ALARM_MASK)                                    +  \
    (ATTR_WATER_SPECIFIC_ALARM_MASK )                             +  \
    (ATTR_HEAT_AND_COOLING_ALARM_MASK)                            +  \
    (ATTR_GAS_ALARM_MASK))

#define CLD_SM_MAX_NUMBER_OF_ATTRIBUTE                              \
    ((CLD_SM_NUMBER_OF_OPTIONAL_ATTRIBUTE)                        +  \
    (CLD_SM_NUMBER_OF_MANDATORY_ATTRIBUTE))

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* SIMPLE_METERING_ATTRIBUTE_COUNT_H */
