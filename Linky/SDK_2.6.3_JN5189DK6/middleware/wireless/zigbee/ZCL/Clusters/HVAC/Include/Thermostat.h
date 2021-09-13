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
 * MODULE:             Thermostat Cluster
 *
 * COMPONENT:          Thermostat.h
 *
 * DESCRIPTION:        Header for Thermostat Cluster
 *
 *****************************************************************************/

#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */

#define HVAC_CLUSTER_ID_THERMOSTAT                                         0x0201

#ifndef CLD_THERMOSTAT_MIN_COOLING_SETPOINT
    #define CLD_THERMOSTAT_MIN_COOLING_SETPOINT                            0x954d
#endif
#ifndef CLD_THERMOSTAT_MAX_COOLING_SETPOINT
    #define CLD_THERMOSTAT_MAX_COOLING_SETPOINT                            0x7fff
#endif
#ifndef CLD_THERMOSTAT_MIN_HEATING_SETPOINT
    #define CLD_THERMOSTAT_MIN_HEATING_SETPOINT                            0x954d
#endif
#ifndef CLD_THERMOSTAT_MAX_HEATING_SETPOINT
    #define CLD_THERMOSTAT_MAX_HEATING_SETPOINT                            0x7fff
#endif

#ifndef CLD_THERMOSTAT_CLUSTER_REVISION
    #define CLD_THERMOSTAT_CLUSTER_REVISION                     3
#endif  

#ifndef CLD_THERMOSTAT_FEATURE_MAP
    #define CLD_THERMOSTAT_FEATURE_MAP                          0
#endif 
/****************************************************************************/
/*             Thermostat Cluster - Optional Attributes                     */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the cluster.                                               */
/****************************************************************************/

/* Enable the optional Outdoor Temperature attribute */
//#define CLD_THERMOSTAT_ATTR_OUTDOOR_TEMPERATURE

/* Enable the optional Occupancy attribute */
//#define CLD_THERMOSTAT_ATTR_OCCUPANCY

/* Enable the optional Minimum Heat Setpoint Limit attribute */
//#define CLD_THERMOSTAT_ATTR_ABS_MIN_HEAT_SETPOINT_LIMIT

/* Enable the optional Maximum Heat Setpoint Limit attribute */
//#define CLD_THERMOSTAT_ATTR_ABS_MAX_HEAT_SETPOINT_LIMIT

/* Enable the optional Minimum Cool Setpoint Limit attribute */
//#define CLD_THERMOSTAT_ATTR_ABS_MIN_COOL_SETPOINT_LIMIT

/* Enable the optional Maximum Cool Setpoint Limit attribute */
//#define CLD_THERMOSTAT_ATTR_ABS_MAX_COOL_SETPOINT_LIMIT

/* Enable the optional PI Cooling Demand attribute */
//#define CLD_THERMOSTAT_ATTR_PI_COOLING_DEMAND

/* Enable the optional PI Heating Demand attribute */
//#define CLD_THERMOSTAT_ATTR_PI_HEATING_DEMAND

/* Enable the optional Local Temperature Calibration attribute */
//#define CLD_THERMOSTAT_ATTR_LOCAL_TEMPERATURE_CALIBRATION

/* Enable the optional Unoccupied Cooling Setpoint attribute */
//#define CLD_THERMOSTAT_ATTR_UNOCCUPIED_COOLING_SETPOINT

/* Enable the optional Unoccupied Heating Setpoint attribute */
//#define CLD_THERMOSTAT_ATTR_UNOCCUPIED_HEATING_SETPOINT

/* Enable the optional Minimum Heat Setpoint Limit attribute */
//#define CLD_THERMOSTAT_ATTR_MIN_HEAT_SETPOINT_LIMIT

/* Enable the optional Maximum Heat Setpoint Limit attribute */
//#define CLD_THERMOSTAT_ATTR_MAX_HEAT_SETPOINT_LIMIT

/* Enable the optional Minimum Cool Setpoint Limit attribute */
//#define CLD_THERMOSTAT_ATTR_MIN_COOL_SETPOINT_LIMIT

/* Enable the optional Maximum Cool Setpoint Limit attribute */
//#define CLD_THERMOSTAT_ATTR_MAX_COOL_SETPOINT_LIMIT

/* Enable the optional Minimum Setpoint Dead Band attribute */
//#define CLD_THERMOSTAT_ATTR_MIN_SETPOINT_DEAD_BAND

/* Enable the optional Remote Sensing attribute */
//#define CLD_THERMOSTAT_ATTR_REMOTE_SENSING

/* Enable the optional Alarm Mask attribute */
//#define CLD_THERMOSTAT_ATTR_ALARM_MASK

/* Enable the optional Attribute Status Reporting */
//#define CLD_THERMOSTAT_ATTR_ATTRIBUTE_REPORTING_STATUS
/* End of optional attributes */


/* Occupancy */
#define CLD_THERMOSTAT_OCCUPANCY_OCCUPIED           (1 << 0)

/* HVAC System Type */

#define CLD_THERMOSTAT_HVAC_COOLING_SYSTEM_STAGE_2    (1 << 0)
#define CLD_THERMOSTAT_HVAC_COOLING_SYSTEM_STAGE_3    (1 << 1)
#define CLD_THERMOSTAT_HVAC_HEATING_SYSTEM_STAGE_2    (1 << 2)
#define CLD_THERMOSTAT_HVAC_HEATING_SYSTEM_STAGE_3    (1 << 3)
#define CLD_THERMOSTAT_HVAC_HEATING_SYSTEM_HEAT_PUMP  (1 << 4)
#define CLD_THERMOSTAT_HVAC_HEATING_FUEL_GAS          (1 << 5)

/* Remote Sensing */
#define CLD_THERMOSTAT_LOCAL_TEMP_SENSED_REMOTELY   (1 << 0)
#define CLD_THERMOSTAT_OUTDOOR_TEMP_SENSED_REMOTELY (1 << 1)
#define CLD_THERMOSTAT_OCCUPANCY_SENSED_REMOTELY    (1 << 2)

/* Alarm Mask*/
#define CLD_THERMOSTAT_ALARM_INIT_FAIL              (1 << 0)
#define CLD_THERMOSTAT_ALARM_HARDWARE_FAIL          (1 << 1)
#define CLD_THERMOSTAT_ALARM_SELF_CAL_FAIL          (1 << 2)

/* Thermostat Programming Operation Mode */

#define CLD_THERMOSTAT_PROGRAMMING_OPERATION_MODE_SCHEDULE    (1 << 0)
#define CLD_THERMOSTAT_PROGRAMMING_OPERATION_MODE_RECOVERY_ON (1 << 1)
#define CLD_THERMOSTAT_PROGRAMMING_OPERATION_MODE_ECONOMY_ON  (1 << 2)

/* Thermostat Running State*/

#define CLD_THERMOSTAT_RUNNING_STATE_HEAT_ON              (1 << 0)
#define CLD_THERMOSTAT_RUNNING_STATE_COOL_ON              (1 << 1)
#define CLD_THERMOSTAT_RUNNING_STATE_FAN_ON               (1 << 2)
#define CLD_THERMOSTAT_RUNNING_STATE_HEAT_2_STAGE_ON      (1 << 3)
#define CLD_THERMOSTAT_RUNNING_STATE_COOL_2_STAGE_ON      (1 << 4)
#define CLD_THERMOSTAT_RUNNING_STATE_FAN_2_STAGE_ON       (1 << 5)
#define CLD_THERMOSTAT_RUNNING_STATE_FAN_3_STAGE_ON       (1 << 6)

/* AC Error Code */

#define CLD_THERMOSTAT_AC_ERROR_CODE_COMPRESSOR_FAIL              (1 << 0)
#define CLD_THERMOSTAT_AC_ERROR_CODE_ROOM_TEMP_SENSOR_FAIL        (1 << 1)
#define CLD_THERMOSTAT_AC_ERROR_CODE_OUTDOOR_TEMP_SENSOR_FAIL     (1 << 2)
#define CLD_THERMOSTAT_AC_ERROR_CODE_INDOOR_COIL_TEMP_SENSOR_FAIL (1 << 3)
#define CLD_THERMOSTAT_AC_ERROR_CODE_FAN_FAIL                     (1 << 4)
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Thermostat Information attribute set attribute ID's (6.3.2.2.1) */
    E_CLD_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE                  = 0x0000, /* Mandatory */
    E_CLD_THERMOSTAT_ATTR_ID_OUTDOOR_TEMPERATURE,
    E_CLD_THERMOSTAT_ATTR_ID_OCCUPANCY,
    E_CLD_THERMOSTAT_ATTR_ID_ABS_MIN_HEAT_SETPOINT_LIMIT,
    E_CLD_THERMOSTAT_ATTR_ID_ABS_MAX_HEAT_SETPOINT_LIMIT,
    E_CLD_THERMOSTAT_ATTR_ID_ABS_MIN_COOL_SETPOINT_LIMIT,
    E_CLD_THERMOSTAT_ATTR_ID_ABS_MAX_COOL_SETPOINT_LIMIT,
    E_CLD_THERMOSTAT_ATTR_ID_PI_COOLING_DEMAND,
    E_CLD_THERMOSTAT_ATTR_ID_PI_HEATING_DEMAND,
    E_CLD_THERMOSTAT_ATTR_ID_HVAC_SYSTEM_TYPE_CONFIGURATION,    

    /* Thermostat settings attribute set attribute ID's (6.3.2.2.2) */
    E_CLD_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE_CALIBRATION  = 0x0010,
    E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_COOLING_SETPOINT,                 /* Mandatory */
    E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_HEATING_SETPOINT,                 /* Mandatory */
    E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_COOLING_SETPOINT,
    E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_HEATING_SETPOINT,
    E_CLD_THERMOSTAT_ATTR_ID_MIN_HEAT_SETPOINT_LIMIT,
    E_CLD_THERMOSTAT_ATTR_ID_MAX_HEAT_SETPOINT_LIMIT,
    E_CLD_THERMOSTAT_ATTR_ID_MIN_COOL_SETPOINT_LIMIT,
    E_CLD_THERMOSTAT_ATTR_ID_MAX_COOL_SETPOINT_LIMIT,
    E_CLD_THERMOSTAT_ATTR_ID_MIN_SETPOINT_DEAD_BAND,
    E_CLD_THERMOSTAT_ATTR_ID_REMOTE_SENSING,
    E_CLD_THERMOSTAT_ATTR_ID_CONTROL_SEQUENCE_OF_OPERATION,             /* Mandatory */
    E_CLD_THERMOSTAT_ATTR_ID_SYSTEM_MODE,                               /* Mandatory */
    E_CLD_THERMOSTAT_ATTR_ID_ALARM_MASK,
    E_CLD_THERMOSTAT_ATTR_ID_THERMOSTAT_RUNNING_MODE,
    
    /* Thermostat Schedule & HVAC Relay attribute set attribute ID's (6.3.2.2.3) */
    E_CLD_THERMOSTAT_ATTR_ID_START_OF_WEEK            = 0x0020,
    E_CLD_THERMOSTAT_ATTR_ID_NUMBER_OF_WEEKLY_TRANSITIONS,
    E_CLD_THERMOSTAT_ATTR_ID_NUMBER_OF_DAILY_TRANSITIONS,
    E_CLD_THERMOSTAT_ATTR_ID_TEMPERATURE_SETPOINT_HOLD,
    E_CLD_THERMOSTAT_ATTR_ID_TEMPERATURE_SETPOINT_HOLD_DURATION,
    E_CLD_THERMOSTAT_ATTR_ID_THERMOSTAT_PROGRAMMING_OPERATION_MODE,
    E_CLD_THERMOSTAT_ATTR_ID_THERMOSTAT_RUNNING_STATE,
    
    /* Thermostat Setpoint ChangeTracking attribute set attribute ID's (6.3.2.2.4) */
    E_CLD_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_SOURCE = 0x0030,
    E_CLD_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_AMOUNT,
    E_CLD_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_SOURCE_TIMESTAMP,
    E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_SETBACK,
    E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_SETBACK_MIN,
    E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_SETBACK_MAX,
    E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_SETBACK,
    E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_SETBACK_MIN,
    E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_SETBACK_MAX,
    E_CLD_THERMOSTAT_ATTR_ID_EMERGENCY_HEAT_DELTA,
    
    /* AC Information attribute set attribute ID's (6.3.2.2.5) */
    E_CLD_THERMOSTAT_ATTR_ID_AC_TYPE = 0X0040,
    E_CLD_THERMOSTAT_ATTR_ID_AC_CAPACITY,
    E_CLD_THERMOSTAT_ATTR_ID_AC_REFRIGERANT_TYPE,
    E_CLD_THERMOSTAT_ATTR_ID_AC_COMPRESSOR_TYPE,
    E_CLD_THERMOSTAT_ATTR_ID_AC_ERROR_CODE,
    E_CLD_THERMOSTAT_ATTR_ID_AC_LOUVER_POSITION,
    E_CLD_THERMOSTAT_ATTR_ID_AC_COIL_TEMPERATURE,
    E_CLD_THERMOSTAT_ATTR_ID_AC_CAPACITY_FORMAT,
        
} teCLD_Thermostat_AttributeID;

/* Command codes */
typedef enum 
{
    E_CLD_THERMOSTAT_CMD_SETPOINT_RAISE_LOWER                     = 0x00,    /* Mandatory */
} teCLD_Thermostat_Command;

/* Control Sequence of Operation */
typedef enum 
{
    E_CLD_THERMOSTAT_CSOO_COOLING_ONLY                      = 0x00,
    E_CLD_THERMOSTAT_CSOO_COOLING_WITH_REHEAT,
    E_CLD_THERMOSTAT_CSOO_HEATING_ONLY,
    E_CLD_THERMOSTAT_CSOO_HEATING_WITH_REHEAT,
    E_CLD_THERMOSTAT_CSOO_COOLING_AND_HEATING_4_PIPES,
    E_CLD_THERMOSTAT_CSOO_COOLING_AND_HEATING_4_PIPES_WITH_REHEAT,
}teCLD_Thermostat_ControlSequenceOfOperation;

/* System Mode */
typedef enum 
{
    E_CLD_THERMOSTAT_SM_OFF                                 = 0x00,
    E_CLD_THERMOSTAT_SM_AUTO,
    E_CLD_THERMOSTAT_SM_COOL                                = 0x03,
    E_CLD_THERMOSTAT_SM_HEAT,
    E_CLD_THERMOSTAT_SM_EMERGENCY_HEATING,
    E_CLD_THERMOSTAT_SM_PRECOOLING,
    E_CLD_THERMOSTAT_SM_FAN_ONLY,
    E_CLD_THERMOSTAT_SM_DRY,
    E_CLD_THERMOSTAT_SM_SLEEP,
}teCLD_Thermostat_SystemMode;

/* Thermostat Running Mode */
typedef enum
{
    E_CLD_THERMOSTAT_TRM_OFF                                = 0x00,
    E_CLD_THERMOSTAT_TRM_COOL                               = 0x03,
    E_CLD_THERMOSTAT_TRM_HEAT                               = 0x04,
}teCLD_Thermostat_ThermostatRunningMode;

/* Start Of Week */
typedef enum
{
    E_CLD_THERMOSTAT_SOW_SUNDAY                             = 0x00,
    E_CLD_THERMOSTAT_SOW_MONDAY,                               
    E_CLD_THERMOSTAT_SOW_TUESDAY,
    E_CLD_THERMOSTAT_SOW_WEDNESDAY,
    E_CLD_THERMOSTAT_SOW_THURSDAY,
    E_CLD_THERMOSTAT_SOW_FRIDAY,
    E_CLD_THERMOSTAT_SOW_SATURDAY,
}teCLD_Thermostat_StartOfWeek;

/* Temperature Setpoint Hold */
typedef enum
{
    E_CLD_THERMOSTAT_TSH_OFF                                = 0x00,
    E_CLD_THERMOSTAT_TSH_ON,
}teCLD_Thermostat_TemperatureSetpointHold;

/* Setpoint Change Source */
typedef enum
{
    E_CLD_THERMOSTAT_SCS_MANUAL                             = 0x00,
    E_CLD_THERMOSTAT_SCS_SCHEDULE,
    E_CLD_THERMOSTAT_SCS_EXTERNAL,
}teCLD_Thermostat_SetpointChangeSource;

/* AC Refrigerant Type */
typedef enum
{
    E_CLD_THERMOSTAT_ACRT_RESERVED                           = 0x00,
    E_CLD_THERMOSTAT_ACRT_R22,
    E_CLD_THERMOSTAT_ACRT_R410A,
    E_CLD_THERMOSTAT_ACRT_R407C,
}teCLD_Thermostat_ACRefrigerantType;

/* AC Compressor Type */
typedef enum
{
    E_CLD_THERMOSTAT_ACCT_RESERVED                           = 0x00,
    E_CLD_THERMOSTAT_ACCT_T1,
    E_CLD_THERMOSTAT_ACCT_T2,
    E_CLD_THERMOSTAT_ACCT_T3,
}teCLD_Thermostat_ACCompressorType;

/* AC Louver Position */
typedef enum
{
    E_CLD_THERMOSTAT_ACLT_FULLY_CLOSED                       = 0x01,
    E_CLD_THERMOSTAT_ACLT_FULLY_OPEN,
    E_CLD_THERMOSTAT_ACLT_QUARTER_OPEN,
    E_CLD_THERMOSTAT_ACLT_HALF_OPEN,
    E_CLD_THERMOSTAT_ACLT_3_QUARTERS_OPEN,    
}teCLD_Thermostat_ACLouverPosition;

/* AC Capacity Format */
typedef enum
{
    E_CLD_THERMOSTAT_ACCF_BTUH                              = 0x00,   
}teCLD_Thermostat_ACCapacityFormat;

/* AC Type */
typedef enum
{
    E_CLD_THERMOSTAT_ACT_RESERVED                           = 0x00,
    E_CLD_THERMOSTAT_ACT_COOLING_FIXED_SPEED,
    E_CLD_THERMOSTAT_ACT_HEAT_PUMP_FIXED_SPEED,
    E_CLD_THERMOSTAT_ACT_COOLING_INVERTER,
    E_CLD_THERMOSTAT_ACT_HEAT_PUMP_INVERTER,
}teCLD_Thermostat_ACType;

/* Setpoint Raise/Lower command modes */
typedef enum
{
    E_CLD_THERMOSTAT_SRLM_HEAT                             = 0x00,
    E_CLD_THERMOSTAT_SRLM_COOL,
    E_CLD_THERMOSTAT_SRLM_BOTH
}teCLD_Thermostat_SetpointRaiseOrLowerMode;

/* Thermostat Cluster */
typedef struct
{
#ifdef THERMOSTAT_SERVER    
    zint16                  i16LocalTemperature;

#ifdef CLD_THERMOSTAT_ATTR_OUTDOOR_TEMPERATURE
    zint16                  i16OutdoorTemperature;
#endif

#ifdef CLD_THERMOSTAT_ATTR_OCCUPANCY
    zbmap8                  u8Occupancy;
#endif

#ifdef CLD_THERMOSTAT_ATTR_ABS_MIN_HEAT_SETPOINT_LIMIT
    zint16                  i16AbsMinHeatSetpointLimit;
#endif

#ifdef CLD_THERMOSTAT_ATTR_ABS_MAX_HEAT_SETPOINT_LIMIT
    zint16                  i16AbsMaxHeatSetpointLimit;
#endif

#ifdef CLD_THERMOSTAT_ATTR_ABS_MIN_COOL_SETPOINT_LIMIT
    zint16                  i16AbsMinCoolSetpointLimit;
#endif

#ifdef CLD_THERMOSTAT_ATTR_ABS_MAX_COOL_SETPOINT_LIMIT
    zint16                  i16AbsMaxCoolSetpointLimit;
#endif

#ifdef CLD_THERMOSTAT_ATTR_PI_COOLING_DEMAND
    zuint8                  u8PICoolingDemand;
#endif

#ifdef CLD_THERMOSTAT_ATTR_PI_HEATING_DEMAND
    zuint8                  u8PIHeatingDemand;
#endif

#ifdef CLD_THERMOSTAT_ATTR_HVAC_SYSTEM_TYPE_CONFIGURATION
    zbmap8                  u8HVACSystemTypeConfiguration;
#endif    
    /* Thermostat settings attribute set attribute ID's (6.3.2.2.2) */
#ifdef CLD_THERMOSTAT_ATTR_LOCAL_TEMPERATURE_CALIBRATION
    zint8                   i8LocalTemperatureCalibration;
#endif
    zint16                  i16OccupiedCoolingSetpoint;

    zint16                  i16OccupiedHeatingSetpoint;
#ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_COOLING_SETPOINT
    zint16                  i16UnoccupiedCoolingSetpoint;
#endif

#ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_HEATING_SETPOINT
    zint16                  i16UnoccupiedHeatingSetpoint;
#endif

#ifdef CLD_THERMOSTAT_ATTR_MIN_HEAT_SETPOINT_LIMIT
    zint16                  i16MinHeatSetpointLimit;
#endif

#ifdef CLD_THERMOSTAT_ATTR_MAX_HEAT_SETPOINT_LIMIT
    zint16                  i16MaxHeatSetpointLimit;
#endif

#ifdef CLD_THERMOSTAT_ATTR_MIN_COOL_SETPOINT_LIMIT
    zint16                  i16MinCoolSetpointLimit;
#endif

#ifdef CLD_THERMOSTAT_ATTR_MAX_COOL_SETPOINT_LIMIT
    zint16                  i16MaxCoolSetpointLimit;
#endif

#ifdef CLD_THERMOSTAT_ATTR_MIN_SETPOINT_DEAD_BAND
    zint8                   i8MinSetpointDeadBand;
#endif

#ifdef CLD_THERMOSTAT_ATTR_REMOTE_SENSING
    zbmap8                  u8RemoteSensing;
#endif
    zenum8                  eControlSequenceOfOperation;
    
    zenum8                  eSystemMode;
#ifdef CLD_THERMOSTAT_ATTR_ALARM_MASK
    zbmap8                  u8AlarmMask;
#endif

#ifdef CLD_THERMOSTAT_ATTR_THERMOSTAT_RUNNING_MODE
    zenum8                  eThermostatRunningMode;
#endif    

#ifdef CLD_THERMOSTAT_ATTR_START_OF_WEEK
    zenum8                  eStartOfWeek;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_NUMBER_OF_WEEKLY_TRANSITIONS
    zuint8                  u8NumberOfWeeklyTransitions;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_NUMBER_OF_DAILY_TRANSITIONS
    zuint8                  u8NumberOfDailyTransitions;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_TEMPERATURE_SETPOINT_HOLD
    zenum8                  eTemperatureSetpointHold;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_TEMPERATURE_SETPOINT_HOLD_DURATION
    zuint16                  u16TemperatureSetpointHoldDuration;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_THERMOSTAT_PROGRAMMING_OPERATION_MODE
    zbmap8                  u8ThermostatProgrammingOperationMode;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_THERMOSTAT_RUNNING_STATE
    zbmap16                  u16ThermostatRunningState;
#endif

#ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_SOURCE
    zenum8                  eSetpointChangeSource;
#endif

#ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_AMOUNT
    zint16                  i16SetpointChangeAmount;
#endif

#ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_SOURCE_TIMESTAMP
    zutctime                  utctSetpointChangeSourceTimestamp;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK
    zuint8                 u8OccupiedSetback;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK_MIN
    zuint8                 u8OccupiedSetbackMin;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK_MAX
    zuint8                 u8OccupiedSetbackMax;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK
    zuint8                 u8UnoccupiedSetback;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK_MIN
    zuint8                 u8UnoccupiedSetbackMin;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK_MAX
    zuint8                 u8UnoccupiedSetbackMax;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_EMERGENCY_HEAT_DELTA
    zuint8                 u8EmergencyHeatDelta;
#endif
    
#ifdef CLD_THERMOSTAT_ATTR_AC_TYPE
    zenum8                 eACType;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_AC_CAPACITY
    zuint16                 u16ACCapacity;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_AC_REFRIGERANT_TYPE
    zenum8                 eACRefrigerantType;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_AC_COMPRESSOR_TYPE
    zenum8                 eACCompressorType;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_AC_ERROR_CODE
    zbmap32                 u32ACErrorCode;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_AC_LOUVER_POSITION
    zenum8                 u8ACLouverPosition;
#endif 
    
#ifdef CLD_THERMOSTAT_ATTR_AC_COIL_TEMPERATURE
    zint16                 i16ACCoilTemperature;
#endif 

#ifdef CLD_THERMOSTAT_ATTR_AC_CAPACITY_FORMAT
    zenum8                 u8ACCapacityFormat;
#endif  
    
#ifdef CLD_THERMOSTAT_ATTR_ATTRIBUTE_REPORTING_STATUS
    zenum8                  u8AttributeReportingStatus;
#endif

#endif
    zbmap32                 u32FeatureMap;
    
    zuint16                 u16ClusterRevision;
} tsCLD_Thermostat;

/* Definition of Command Thermostat Setpoint Raise or lower Structure */
typedef struct
{
    zenum8                 eMode;
    zint8                  i8Amount;
}tsCLD_Thermostat_SetpointRaiseOrLowerPayload;


/* Definition of Thermostat Call back Event Structure */
typedef struct
{
    uint8                                               u8CommandId;
    union
    {
        tsCLD_Thermostat_SetpointRaiseOrLowerPayload    *psSetpointRaiseOrLowerPayload;
    } uMessage;
} tsCLD_ThermostatCallBackMessage;

/* Custom data structure */
typedef struct
{
    tsZCL_ReceiveEventAddress         sReceiveEventAddress;
    tsZCL_CallBackEvent               sCustomCallBackEvent;
    tsCLD_ThermostatCallBackMessage   sCallBackMessage;
} tsCLD_ThermostatCustomDataStructure;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ThermostatCreateThermostat(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                  bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits,
                tsCLD_ThermostatCustomDataStructure     *psCustomDataStructure);

#ifdef THERMOSTAT_SERVER
PUBLIC teZCL_Status eCLD_ThermostatSetAttribute(
                uint8                                   u8SourceEndPointId,
                uint8                                   u8AttributeId,
                int16                                   i16AttributeValue); 

PUBLIC teZCL_Status eCLD_ThermostatStartReportingLocalTemperature(
                uint8                                   u8SourceEndPointId,
                uint8                                   u8DstEndPointId,
                uint64                                  u64DstAddr,
                uint16                                  u16MinReportInterval,
                uint16                                  u16MaxReportInterval,
                int16                                   i16ReportableChange);
#endif       

#ifdef THERMOSTAT_CLIENT
PUBLIC teZCL_Status eCLD_ThermostatCommandSetpointRaiseOrLowerSend(
                    uint8                                           u8SourceEndPointId,
                    uint8                                           u8DestinationEndPointId,
                    tsZCL_Address                                   *psDestinationAddress,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_Thermostat_SetpointRaiseOrLowerPayload    *psPayload);
#endif       
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern const tsZCL_AttributeDefinition asCLD_ThermostatClusterAttributeDefinitions[];
extern tsZCL_ClusterDefinition sCLD_Thermostat;
extern uint8 au8ThermostatAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* THERMOSTAT_H */
