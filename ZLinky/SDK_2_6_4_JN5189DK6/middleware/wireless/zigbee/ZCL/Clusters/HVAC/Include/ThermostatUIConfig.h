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
 * MODULE:             ThermostatUIConfigUIConfig Cluster
 *
 * COMPONENT:          ThermostatUIConfigUIConfig.h
 *
 * DESCRIPTION:        Header for ThermostatUIConfigUIConfig Cluster
 *
 *****************************************************************************/

#ifndef THERMOSTAT_UI_CONFIG_H
#define THERMOSTAT_UI_CONFIG_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*       ThermostatUIConfigUIConfig Cluster - Optional Attributes              */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the temperature measurement cluster.                       */
/****************************************************************************/

/* End of optional attributes */


/* Cluster ID's */
#define HVAC_CLUSTER_ID_THERMOSTAT_UI_CONFIG                                             0x0204

#ifndef CLD_THERMOSTAT_UI_CONFIG_CLUSTER_REVISION
    #define CLD_THERMOSTAT_UI_CONFIG_CLUSTER_REVISION                     1
#endif  

#ifndef CLD_THERMOSTAT_UI_CONFIG_FEATURE_MAP
    #define CLD_THERMOSTAT_UI_CONFIG_FEATURE_MAP                          0
#endif 
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* ThermostatUIConfigUIConfig attribute set attribute ID's (6.6.2.2) */
    E_CLD_THERMOSTAT_UI_CONFIG_ATTR_ID_TEMPERATURE_DISPLAY_MODE               = 0x0000,         /* Mandatory */
    E_CLD_THERMOSTAT_UI_CONFIG_ATTR_ID_KEYPAD_LOCKOUT,                                          /* Mandatory */
} teCLD_ThermostatUIConfig_AttributeID;

/* Temperature Display */
typedef enum 
{
    E_CLD_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_CELSIUS                 = 0x00,
    E_CLD_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_FAHRENHEIT
} teCLD_ThermostatUIConfig_TemperatureDisplay;

/* Keypad Lockout */
typedef enum 
{
    E_CLD_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_NO_LOCKOUT                               = 0x00,
    E_CLD_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_1_LOCKOUT,
    E_CLD_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_2_LOCKOUT,
    E_CLD_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_3_LOCKOUT,
    E_CLD_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_4_LOCKOUT,
    E_CLD_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_5_LOCKOUT
} teCLD_ThermostatUIConfig_KeyPadLockout;

/* ThermostatUIConfig Cluster */
typedef struct
{
#ifdef THERMOSTAT_UI_CONFIG_SERVER    
    zenum8                 eTemperatureDisplayMode;
    
    zenum8                 eKeypadLockout;
#endif    
    zbmap32                u32FeatureMap;
    
    zuint16                u16ClusterRevision;
} tsCLD_ThermostatUIConfig;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ThermostatUIConfigCreateThermostatUIConfig(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                  bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits);
#ifdef THERMOSTAT_UI_CONFIG_SERVER
PUBLIC teZCL_Status eCLD_ThermostatUIConfigConvertTemp(
                uint8                                   u8SourceEndPointId,
				bool_t                                    bConvertFToC,
                int16                                   *pi16Temperature); 
#endif                
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern const tsZCL_AttributeDefinition asCLD_ThermostatUIConfigClusterAttributeDefinitions[];
extern tsZCL_ClusterDefinition sCLD_ThermostatUIConfig;
extern uint8 au8ThermostatUIConfigAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* THERMOSTAT_UI_CONFIG_H */
