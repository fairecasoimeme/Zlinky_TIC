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
 * COMPONENT:          Thermostat.c
 *
 * DESCRIPTION:        Thermostat cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"
#include "zps_apl_zdo.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"
#include "string.h"
#include "Thermostat.h"
#include "Thermostat_internal.h"

#include "dbg.h"

#ifdef DEBUG_CLD_THERMOSTAT
#define TRACE_THERMOSTAT    TRUE
#else
#define TRACE_THERMOSTAT    FALSE
#endif



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_THERMOSTAT) && !defined(THERMOSTAT_SERVER) && !defined(THERMOSTAT_CLIENT)
#error You Must Have either THERMOSTAT_SERVER and/or THERMOSTAT_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_THERMOSTAT
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_ThermostatClusterCommandDefinitions[] = {
        {E_CLD_THERMOSTAT_CMD_SETPOINT_RAISE_LOWER,                E_ZCL_CF_RX}
    };
#endif

const tsZCL_AttributeDefinition asCLD_ThermostatClusterAttributeDefinitions[] = {
#ifdef THERMOSTAT_SERVER
        {E_CLD_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE,                (E_ZCL_AF_RD|E_ZCL_AF_RP),                    E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16LocalTemperature),           0},   /* Mandatory */

    #ifdef CLD_THERMOSTAT_ATTR_OUTDOOR_TEMPERATURE
        {E_CLD_THERMOSTAT_ATTR_ID_OUTDOOR_TEMPERATURE,              E_ZCL_AF_RD,                    E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16OutdoorTemperature),         0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_OCCUPANCY
        {E_CLD_THERMOSTAT_ATTR_ID_OCCUPANCY,                        E_ZCL_AF_RD,                    E_ZCL_BMAP8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8Occupancy),                   0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_ABS_MIN_HEAT_SETPOINT_LIMIT
        {E_CLD_THERMOSTAT_ATTR_ID_ABS_MIN_HEAT_SETPOINT_LIMIT,      E_ZCL_AF_RD,                    E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16AbsMinHeatSetpointLimit),    0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_ABS_MAX_HEAT_SETPOINT_LIMIT
        {E_CLD_THERMOSTAT_ATTR_ID_ABS_MAX_HEAT_SETPOINT_LIMIT,      E_ZCL_AF_RD,                    E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16AbsMaxHeatSetpointLimit),    0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_ABS_MIN_COOL_SETPOINT_LIMIT
        {E_CLD_THERMOSTAT_ATTR_ID_ABS_MIN_COOL_SETPOINT_LIMIT,      E_ZCL_AF_RD,                    E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16AbsMinCoolSetpointLimit),    0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_ABS_MAX_COOL_SETPOINT_LIMIT
        {E_CLD_THERMOSTAT_ATTR_ID_ABS_MAX_COOL_SETPOINT_LIMIT,      E_ZCL_AF_RD,                    E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16AbsMaxCoolSetpointLimit),    0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_PI_COOLING_DEMAND
        {E_CLD_THERMOSTAT_ATTR_ID_PI_COOLING_DEMAND,                (E_ZCL_AF_RD|E_ZCL_AF_RP),                    E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8PICoolingDemand),             0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_PI_HEATING_DEMAND
        {E_CLD_THERMOSTAT_ATTR_ID_PI_HEATING_DEMAND,                (E_ZCL_AF_RD|E_ZCL_AF_RP),                    E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8PIHeatingDemand),             0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_HVAC_SYSTEM_TYPE_CONFIGURATION
        {E_CLD_THERMOSTAT_ATTR_ID_HVAC_SYSTEM_TYPE_CONFIGURATION,   (E_ZCL_AF_RD|E_ZCL_AF_WR),                    E_ZCL_BMAP8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8HVACSystemTypeConfiguration), 0},   /* Optional  */
    #endif        
        /* Thermostat settings attribute set attribute ID's (6.3.2.2.2) */
    #ifdef CLD_THERMOSTAT_ATTR_LOCAL_TEMPERATURE_CALIBRATION
        {E_CLD_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE_CALIBRATION,    (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT8,     (uint32)(&((tsCLD_Thermostat*)(0))->i8LocalTemperatureCalibration), 0},   /* Optional  */
    #endif
        {E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_COOLING_SETPOINT,        (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16OccupiedCoolingSetpoint),    0},   /* Mandatory */

        {E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_HEATING_SETPOINT,        (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16OccupiedHeatingSetpoint),    0},   /* Mandatory */

    #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_COOLING_SETPOINT
        {E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_COOLING_SETPOINT,      (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16UnoccupiedCoolingSetpoint),  0},
    #endif
    #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_HEATING_SETPOINT
        {E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_HEATING_SETPOINT,      (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16UnoccupiedHeatingSetpoint),  0},
    #endif
    #ifdef CLD_THERMOSTAT_ATTR_MIN_HEAT_SETPOINT_LIMIT
        {E_CLD_THERMOSTAT_ATTR_ID_MIN_HEAT_SETPOINT_LIMIT,          (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16MinHeatSetpointLimit),       0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_MAX_HEAT_SETPOINT_LIMIT
        {E_CLD_THERMOSTAT_ATTR_ID_MAX_HEAT_SETPOINT_LIMIT,          (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16MaxHeatSetpointLimit),       0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_MIN_COOL_SETPOINT_LIMIT
        {E_CLD_THERMOSTAT_ATTR_ID_MIN_COOL_SETPOINT_LIMIT,          (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16MinCoolSetpointLimit),       0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_MAX_COOL_SETPOINT_LIMIT
        {E_CLD_THERMOSTAT_ATTR_ID_MAX_COOL_SETPOINT_LIMIT,          (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16MaxCoolSetpointLimit),       0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_MIN_SETPOINT_DEAD_BAND
        {E_CLD_THERMOSTAT_ATTR_ID_MIN_SETPOINT_DEAD_BAND,           (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_INT8,     (uint32)(&((tsCLD_Thermostat*)(0))->i8MinSetpointDeadBand),         0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_REMOTE_SENSING
        {E_CLD_THERMOSTAT_ATTR_ID_REMOTE_SENSING,                   (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_BMAP8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8RemoteSensing),               0},   /* Optional  */
    #endif
        {E_CLD_THERMOSTAT_ATTR_ID_CONTROL_SEQUENCE_OF_OPERATION,    (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_ENUM8,    (uint32)(&((tsCLD_Thermostat*)(0))->eControlSequenceOfOperation),   0},   /* Mandatory */

        {E_CLD_THERMOSTAT_ATTR_ID_SYSTEM_MODE,                      (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_ENUM8,    (uint32)(&((tsCLD_Thermostat*)(0))->eSystemMode),                   0},   /* Mandatory */
    #ifdef CLD_THERMOSTAT_ATTR_ALARM_MASK
        {E_CLD_THERMOSTAT_ATTR_ID_ALARM_MASK,                       E_ZCL_AF_RD,                    E_ZCL_BMAP8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8AlarmMask),                   0},   /* Optional  */
    #endif
        
    #ifdef CLD_THERMOSTAT_ATTR_THERMOSTAT_RUNNING_MODE
        {E_CLD_THERMOSTAT_ATTR_ID_THERMOSTAT_RUNNING_MODE,          E_ZCL_AF_RD,                    E_ZCL_ENUM8,    (uint32)(&((tsCLD_Thermostat*)(0))->eThermostatRunningMode),        0},   /* Optional  */
    #endif    
        
    #ifdef CLD_THERMOSTAT_ATTR_THERMOSTAT_START_OF_WEEK
        {E_CLD_THERMOSTAT_ATTR_ID_START_OF_WEEK,                    E_ZCL_AF_RD,                    E_ZCL_ENUM8,    (uint32)(&((tsCLD_Thermostat*)(0))->eStartOfWeek),                  0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_NUMBER_OF_WEEKLY_TRANSITIONS
        {E_CLD_THERMOSTAT_ATTR_ID_NUMBER_OF_WEEKLY_TRANSITIONS,     E_ZCL_AF_RD,                    E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8NumberOfWeeklyTransitions),   0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_NUMBER_OF_DAILY_TRANSITIONS
        {E_CLD_THERMOSTAT_ATTR_ID_NUMBER_OF_DAILY_TRANSITIONS,      E_ZCL_AF_RD,                    E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8NumberOfDailyTransitions),    0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_TEMPERATURE_SETPOINT_HOLD
        {E_CLD_THERMOSTAT_ATTR_ID_TEMPERATURE_SETPOINT_HOLD,        (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_ENUM8,    (uint32)(&((tsCLD_Thermostat*)(0))->eTemperatureSetpointHold),      0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_TEMPERATURE_SETPOINT_HOLD_DURATION
        {E_CLD_THERMOSTAT_ATTR_ID_TEMPERATURE_SETPOINT_HOLD_DURATION,  (E_ZCL_AF_RD|E_ZCL_AF_WR),   E_ZCL_UINT16,   (uint32)(&((tsCLD_Thermostat*)(0))->u16TemperatureSetpointHoldDuration),      0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_PROGRAMMING_OPERATION_MODE
        {E_CLD_THERMOSTAT_ATTR_ID_PROGRAMMING_OPERATION_MODE,       (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_RP),   E_ZCL_BMAP8,   (uint32)(&((tsCLD_Thermostat*)(0))->u8ThermostatProgrammingOperationMode),      0},   /* Optional  */
    #endif

    #ifdef CLD_THERMOSTAT_ATTR_THERMOSTAT_RUNNING_STATE
        {E_CLD_THERMOSTAT_ATTR_ID_THERMOSTAT_RUNNING_STATE,         (E_ZCL_AF_RD|E_ZCL_AF_WR|E_ZCL_AF_RP),   E_ZCL_BMAP16,  (uint32)(&((tsCLD_Thermostat*)(0))->u16ThermostatRunningState),       0},   /* Optional  */
    #endif  

    #ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_SOURCE
        {E_CLD_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_SOURCE,           E_ZCL_AF_RD,                    E_ZCL_ENUM8,    (uint32)(&((tsCLD_Thermostat*)(0))->eSetpointChangeSource),         0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_AMOUNT
        {E_CLD_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_AMOUNT,           E_ZCL_AF_RD,                    E_ZCL_INT16,    (uint32)(&((tsCLD_Thermostat*)(0))->i16SetpointChangeAmount),       0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_SOURCE_TIMESTAMP
        {E_CLD_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_SOURCE_TIMESTAMP, E_ZCL_AF_RD,                    E_ZCL_UTCT,     (uint32)(&((tsCLD_Thermostat*)(0))->utctSetpointChangeSourceTimestamp),       0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK
        {E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_SETBACK,                 (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8OccupiedSetback),             0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK_MIN
        {E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_SETBACK_MIN,             E_ZCL_AF_RD,                    E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8OccupiedSetbackMin),          0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK_MAX
        {E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_SETBACK_MAX,             E_ZCL_AF_RD,                    E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8OccupiedSetbackMax),          0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK
        {E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_SETBACK,               (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8UnoccupiedSetback),           0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK_MIN
        {E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_SETBACK_MIN,           E_ZCL_AF_RD,                    E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8UnoccupiedSetbackMin),        0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK_MAX
        {E_CLD_THERMOSTAT_ATTR_ID_UNOCCUPIED_SETBACK_MAX,           E_ZCL_AF_RD,                    E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8UnoccupiedSetbackMax),        0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_EMERGENCY_HEAT_DELTA
        {E_CLD_THERMOSTAT_ATTR_ID_EMERGENCY_HEAT_DELTA,            (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_UINT8,    (uint32)(&((tsCLD_Thermostat*)(0))->u8EmergencyHeatDelta),          0},   /* Optional  */
    #endif         

    #ifdef CLD_THERMOSTAT_ATTR_AC_TYPE 
        {E_CLD_THERMOSTAT_ATTR_ID_AC_TYPE,                        (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_ENUM8,     (uint32)(&((tsCLD_Thermostat*)(0))->eACType),                       0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_AC_CAPACITY
        {E_CLD_THERMOSTAT_ATTR_ID_AC_CAPACITY,                    (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_UINT16,    (uint32)(&((tsCLD_Thermostat*)(0))->u16ACCapacity),                 0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_AC_REFRIGERANT_TYPE
        {E_CLD_THERMOSTAT_ATTR_ID_AC_REFRIGERANT_TYPE,            (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_ENUM8,     (uint32)(&((tsCLD_Thermostat*)(0))->eACRefrigerantType),            0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_AC_COMPRESSOR_TYPE 
        {E_CLD_THERMOSTAT_ATTR_ID_AC_COMPRESSOR_TYPE,             (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_ENUM8,     (uint32)(&((tsCLD_Thermostat*)(0))->eACCompressorType),             0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_AC_ERROR_CODE 
        {E_CLD_THERMOSTAT_ATTR_ID_AC_ERROR_CODE,                  (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_BMAP32,    (uint32)(&((tsCLD_Thermostat*)(0))->u32ACErrorCode),                0},   /* Optional  */
    #endif 
        
    #ifdef CLD_THERMOSTAT_ATTR_AC_LOUVER_POSITION
        {E_CLD_THERMOSTAT_ATTR_ID_AC_LOUVER_POSITION,             (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_ENUM8,     (uint32)(&((tsCLD_Thermostat*)(0))->u8ACLouverPosition),            0},   /* Optional  */
    #endif 

    #ifdef CLD_THERMOSTAT_ATTR_AC_COIL_TEMPERATURE
        {E_CLD_THERMOSTAT_ATTR_ID_AC_COIL_TEMPERATURE,             E_ZCL_AF_RD,                    E_ZCL_INT16,     (uint32)(&((tsCLD_Thermostat*)(0))->i16ACCoilTemperature),          0},   /* Optional  */
    #endif 
        
    #ifdef CLD_THERMOSTAT_ATTR_AC_CAPACITY_FORMAT
        {E_CLD_THERMOSTAT_ATTR_ID_AC_CAPACITY_FORMAT,             (E_ZCL_AF_RD|E_ZCL_AF_WR),       E_ZCL_ENUM8,     (uint32)(&((tsCLD_Thermostat*)(0))->u8ACCapacityFormat),            0},   /* Optional  */
    #endif         
#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                          (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_BMAP32,   (uint32)(&((tsCLD_Thermostat*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                     (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_UINT16,   (uint32)(&((tsCLD_Thermostat*)(0))->u16ClusterRevision),            0},   /* Mandatory  */
           
    #if (defined THERMOSTAT_SERVER) && (defined CLD_THERMOSTAT_ATTR_ATTRIBUTE_REPORTING_STATUS)
        {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,           (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_ENUM8,     (uint32)(&((tsCLD_Thermostat*)(0))->u8AttributeReportingStatus),    0},  /* Optional */
    #endif
};

tsZCL_ClusterDefinition sCLD_Thermostat = {
        HVAC_CLUSTER_ID_THERMOSTAT,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_ThermostatClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_ThermostatClusterAttributeDefinitions,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
            ,
            (sizeof(asCLD_ThermostatClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_ThermostatClusterCommandDefinitions 
        #endif        
};

uint8 au8ThermostatAttributeControlBits[(sizeof(asCLD_ThermostatClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_ThermostatCreateThermostat
 **
 ** DESCRIPTION:
 ** Creates a thermostat cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ThermostatCreateThermostat(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                  bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits,
                tsCLD_ThermostatCustomDataStructure     *psCustomDataStructure)
{

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL)  ||
           (psCustomDataStructure==NULL))
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
                                   psCustomDataStructure,
                                   eCLD_ThermostatCommandHandler);      

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    
        /* Initialise attributes */

        if(pvEndPointSharedStructPtr != NULL)
        {
        #ifdef THERMOSTAT_SERVER
            /* Thermostat information attribute set attribute ID's (6.3.2.2.1) */
            ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16LocalTemperature = 0x8000;
                
            #ifdef CLD_THERMOSTAT_ATTR_OUTDOOR_TEMPERATURE
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16OutdoorTemperature = 0x8000;
            #endif
                
            #ifdef CLD_THERMOSTAT_ATTR_OCCUPANCY
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8Occupancy = 1;
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_ABS_MIN_HEAT_SETPOINT_LIMIT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16AbsMinHeatSetpointLimit = 0x02bc; // 7 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_ABS_MAX_HEAT_SETPOINT_LIMIT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16AbsMaxHeatSetpointLimit = 0x0bb8; // 30 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_ABS_MIN_COOL_SETPOINT_LIMIT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16AbsMinCoolSetpointLimit = 0x0640; // 16 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_ABS_MAX_COOL_SETPOINT_LIMIT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16AbsMaxCoolSetpointLimit = 0x0c80; // 32 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_HVAC_SYSTEM_TYPE_CONFIGURATION
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8HVACSystemTypeConfiguration = 0;
            #endif  
        
                /* Thermostat settings attribute set attribute ID's (6.3.2.2.2) */
            #ifdef CLD_THERMOSTAT_ATTR_LOCAL_TEMPERATURE_CALIBRATION
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i8LocalTemperatureCalibration = 0; // 0 Deg C
            #endif
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16OccupiedCoolingSetpoint = 0x0a28; // 26 Deg C
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16OccupiedHeatingSetpoint = 0x07d0;  // 20 Deg C

            #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_COOLING_SETPOINT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16UnoccupiedCoolingSetpoint = 0x0a28; // 26 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_HEATING_SETPOINT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16UnoccupiedHeatingSetpoint = 0x07d0; // 20 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_MIN_HEAT_SETPOINT_LIMIT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16MinHeatSetpointLimit = 0x02bc; // 7 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_MAX_HEAT_SETPOINT_LIMIT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16MaxHeatSetpointLimit = 0x0bb8; // 30 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_MIN_COOL_SETPOINT_LIMIT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16MinCoolSetpointLimit = 0x02bc; // 7 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_MAX_COOL_SETPOINT_LIMIT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16MaxCoolSetpointLimit = 0x0bb8; // 30 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_MIN_SETPOINT_DEAD_BAND
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i8MinSetpointDeadBand = 0x19; // 2.5 Deg C
            #endif

            #ifdef CLD_THERMOSTAT_ATTR_REMOTE_SENSING
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8RemoteSensing = 0;
            #endif
                
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->eControlSequenceOfOperation = E_CLD_THERMOSTAT_CSOO_COOLING_AND_HEATING_4_PIPES;
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->eSystemMode = E_CLD_THERMOSTAT_SM_AUTO;
            #ifdef CLD_THERMOSTAT_ATTR_ALARM_MASK
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8AlarmMask = 0;
            #endif
                
            #ifdef CLD_THERMOSTAT_ATTR_THERMOSTAT_RUNNING_MODE
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->eThermostatRunningMode = E_CLD_THERMOSTAT_TRM_OFF;
            #endif 
                
            #ifdef CLD_THERMOSTAT_ATTR_NUMBER_OF_WEEKLY_TRANSITIONS
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8NumberOfWeeklyTransitions = 0x00;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_NUMBER_OF_DAILY_TRANSITIONS
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8NumberOfDailyTransitions = 0x00;
            #endif 
                
            #ifdef CLD_THERMOSTAT_ATTR_TEMPERATURE_SETPOINT_HOLD
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->eTemperatureSetpointHold = E_CLD_THERMOSTAT_TSH_OFF;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_TEMPERATURE_SETPOINT_HOLD_DURATION
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u16TemperatureSetpointHoldDuration = 0xFFFF;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_PROGRAMMING_OPERATION_MODE
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8ThermostatProgrammingOperationMode = 0;
            #endif
                
            #ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_SOURCE
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->eSetpointChangeSource = E_CLD_THERMOSTAT_SCS_MANUAL;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_AMOUNT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->i16SetpointChangeAmount = 0x8000;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_SETPOINT_CHANGE_SOURCE_TIMESTAMP
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->utctSetpointChangeSourceTimestamp = 0x00000000;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8OccupiedSetback = 0xFF;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK_MIN
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8OccupiedSetbackMin = 0xFF;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_OCCUPIED_SETBACK_MAX
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8OccupiedSetbackMax = 0xFF;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8UnoccupiedSetback = 0xFF;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK_MIN
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8UnoccupiedSetbackMin = 0xFF;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_UNOCCUPIED_SETBACK_MAX
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8UnoccupiedSetbackMax = 0xFF;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_EMERGENCY_HEAT_DELTA
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8EmergencyHeatDelta = 0xFF;
            #endif         
        
            #ifdef CLD_THERMOSTAT_ATTR_AC_TYPE 
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->eACType = E_CLD_THERMOSTAT_ACT_RESERVED;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_AC_CAPACITY
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u16ACCapacity = 0x0000;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_AC_REFRIGERANT_TYPE
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->eACRefrigerantType = E_CLD_THERMOSTAT_ACRT_RESERVED;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_AC_COMPRESSOR_TYPE 
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->eACCompressorType = E_CLD_THERMOSTAT_ACCT_RESERVED;
            #endif 
        
            #ifdef CLD_THERMOSTAT_ATTR_AC_ERROR_CODE 
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u32ACErrorCode = 0;
            #endif 
                
            #ifdef CLD_THERMOSTAT_ATTR_AC_LOUVER_POSITION
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8ACLouverPosition = E_CLD_THERMOSTAT_ACLT_FULLY_CLOSED;
            #endif 
                
            #ifdef CLD_THERMOSTAT_ATTR_AC_CAPACITY_FORMAT
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u8ACCapacityFormat = E_CLD_THERMOSTAT_ACCF_BTUH;
            #endif                    
       #endif
                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_THERMOSTAT_FEATURE_MAP;

                ((tsCLD_Thermostat*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_THERMOSTAT_CLUSTER_REVISION;
        }

    /* As this cluster has reportable attributes enable default reporting */
    vZCL_SetDefaultReporting(psClusterInstance);
    
    return E_ZCL_SUCCESS;

}

#ifdef THERMOSTAT_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_ThermostatSetAttribute
 **
 ** DESCRIPTION:
 ** Used by cluster to set its attributes
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 ** uint8                       u8AttributeId               Attribute Id
 ** int16                       i16AttributeValue           Attribute Value
 **
 ** RETURN:
 ** teZCL_Status
 ** 
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_ThermostatSetAttribute(
                                            uint8               u8SourceEndPointId,
                                            uint8               u8AttributeId,
                                            int16               i16AttributeValue)  
{
    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ThermostatCustomDataStructure *psCustomDataStructure;

    eStatus = eZCL_FindCluster(HVAC_CLUSTER_ID_THERMOSTAT, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
        
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    switch(u8AttributeId)
    {
    case E_CLD_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE:
    {
        if(i16AttributeValue >= (int16)(CLD_THERMOSTAT_MIN_COOLING_SETPOINT) && i16AttributeValue <= (int16)(CLD_THERMOSTAT_MAX_HEATING_SETPOINT))
            ((tsCLD_Thermostat *)(psClusterInstance->pvEndPointSharedStructPtr))->i16LocalTemperature = i16AttributeValue;
        else
            eStatus =  E_ZCL_ERR_INVALID_VALUE;
        break;
    }
    case E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_COOLING_SETPOINT:
    {
        if(i16AttributeValue >= (int16)(CLD_THERMOSTAT_MIN_COOLING_SETPOINT) && i16AttributeValue <= (int16)(CLD_THERMOSTAT_MAX_COOLING_SETPOINT))
            ((tsCLD_Thermostat *)(psClusterInstance->pvEndPointSharedStructPtr))->i16OccupiedCoolingSetpoint = i16AttributeValue;
        else
            eStatus = E_ZCL_ERR_INVALID_VALUE;
        break;
    }
    case E_CLD_THERMOSTAT_ATTR_ID_OCCUPIED_HEATING_SETPOINT:
    {
        if(i16AttributeValue >= (int16)(CLD_THERMOSTAT_MIN_HEATING_SETPOINT) && i16AttributeValue <= (int16)(CLD_THERMOSTAT_MAX_HEATING_SETPOINT))
            ((tsCLD_Thermostat *)(psClusterInstance->pvEndPointSharedStructPtr))->i16OccupiedHeatingSetpoint = i16AttributeValue;
        else
            eStatus = E_ZCL_ERR_INVALID_VALUE;
        break;
    }
    default:
        eStatus = E_ZCL_DENY_ATTRIBUTE_ACCESS;
        break;
    }
    
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    
    return eStatus;

}

/****************************************************************************
 **
 ** NAME:       eCLD_ThermostatStartReportingLocalTemperature
 **
 ** DESCRIPTION:
 ** Used by cluster to start reporting its LocalTemperature attribute
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Src EndPoint Id
 ** uint8                       u8DstEndPointId             Dst EndPoint Id
 ** uint64                      u64DstAddr                  Destination Address
 ** uint16                      u16MinReportInterval        Minimum Report Interval
 ** uint16                      u16MaxReportInterval        Maximum Report Interval
 ** int16                       i16ReportableChange         Reportable Change
 **
 ** RETURN:
 ** teZCL_Status
 ** 
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_ThermostatStartReportingLocalTemperature(
                                            uint8               u8SourceEndPointId,
                                            uint8               u8DstEndPointId,
                                            uint64              u64DstAddr,
                                            uint16              u16MinReportInterval,
                                            uint16              u16MaxReportInterval,
                                            int16               i16ReportableChange)  
{
    teZCL_Status eStatus;
    ZPS_teStatus eReturnCode = ZPS_APL_APS_E_INVALID_PARAMETER;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ThermostatCustomDataStructure *psCustomDataStructure;
    tsZCL_AttributeReportingConfigurationRecord    sAttributeReportingConfigurationRecord;
   // ZPS_tsAplApsmeBindingTableEntry sNewEntry;

    eStatus = eZCL_FindCluster(HVAC_CLUSTER_ID_THERMOSTAT, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
        
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    /*For OnOff attribute in ON Off Cluster*/
    sAttributeReportingConfigurationRecord.eAttributeDataType = E_ZCL_INT16;
    sAttributeReportingConfigurationRecord.u16AttributeEnum = E_CLD_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE;
    sAttributeReportingConfigurationRecord.u16MaximumReportingInterval=u16MaxReportInterval;
    sAttributeReportingConfigurationRecord.u16MinimumReportingInterval=u16MinReportInterval;
    sAttributeReportingConfigurationRecord.u16TimeoutPeriodField=0;
    sAttributeReportingConfigurationRecord.u8DirectionIsReceived=0;
    sAttributeReportingConfigurationRecord.uAttributeReportableChange.zint16ReportableChange=i16ReportableChange;
    
   /* sNewEntry.u16ClusterId = HVAC_CLUSTER_ID_THERMOSTAT;
    sNewEntry.u8SourceEndpoint = u8SourceEndPointId;
    sNewEntry.u8DstAddrMode = E_ZCL_AM_IEEE;
    sNewEntry.uDstAddress.u64Addr = u64DstAddr;
    sNewEntry.u8DestinationEndPoint = u8DstEndPointId;
    eReturnCode = zps_eAplApsmeBindReqRsp(ZPS_pvAplZdoGetAplHandle(), ZPS_u64NwkNibGetExtAddr(ZPS_pvAplZdoGetNwkHandle()), &sNewEntry);*/
    eReturnCode = ZPS_eAplZdoBind(HVAC_CLUSTER_ID_THERMOSTAT,
    					u8SourceEndPointId,
    					ZPS_u16AplZdoLookupAddr(u64DstAddr),
    					u64DstAddr,
    					u8DstEndPointId);

    if(eReturnCode == ZPS_E_SUCCESS) 
    {    
        eStatus = eZCL_SetReportableFlag(u8SourceEndPointId,HVAC_CLUSTER_ID_THERMOSTAT, TRUE, FALSE,E_CLD_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE);
        if(eStatus == E_ZCL_SUCCESS)
            eStatus = eZCL_CreateLocalReport(u8SourceEndPointId,HVAC_CLUSTER_ID_THERMOSTAT,FALSE,TRUE,&sAttributeReportingConfigurationRecord);
    }else{
        eStatus = E_ZCL_FAIL;
    }

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    
    return eStatus;

}
#endif /*#define THERMOSTAT_SERVER */
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


#endif /*#define  CLD_THERMOSTAT */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

