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
 * MODULE:             THERMOSTAT DEVICE
 *
 * COMPONENT:          thermostat_device.h
 *
 * DESCRIPTION:        Header for ZigBee Thermostat profile functions
 *
 *****************************************************************************/

#ifndef THERMOSTAT_DEVICE_H
#define THERMOSTAT_DEVICE_H

#if defined __cplusplus
extern "C" {
#endif

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "Basic.h"
#include "Identify.h"
#ifdef CLD_POWER_CONFIGURATION
#include "PowerConfiguration.h"
#endif
#ifdef CLD_DEVICE_TEMPERATURE_CONFIGURATION
#include "DeviceTemperatureConfiguration.h"
#endif
#ifdef CLD_ALARMS
#include "Alarms.h"
#endif
#include "Groups.h"
#include "Scenes.h"
#include "Thermostat.h"
#include "ThermostatUIConfig.h"
#ifdef CLD_TEMPERATURE_MEASUREMENT
#include "TemperatureMeasurement.h"
#endif
#ifdef CLD_RELATIVE_HUMIDITY_MEASUREMENT
#include "RelativeHumidityMeasurement.h"
#endif
#ifdef CLD_OCCUPANCY_SENSING
#include "OccupancySensing.h"
#endif
#ifdef CLD_OTA
#include "OTA.h"
#endif
#include "Time.h"
#ifdef CLD_POLL_CONTROL
    #include "PollControl.h"
#endif
#ifdef CLD_ELECTRICAL_MEASUREMENT
	#include "ElectricalMeasurement.h"
#endif
#ifdef CLD_FAN_CONTROL
    #include "FanControl.h"
#endif
#ifdef CLD_DIAGNOSTICS
   #include "Diagnostics.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define HVAC_THERMOSTAT_DEVICE_ID                                       0x0301
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Holds cluster instances */
typedef struct 
{
    /* All HA devices have 2 mandatory clusters - Basic(server) and Identify(server) */
#if (defined CLD_BASIC) && (defined BASIC_SERVER)
    tsZCL_ClusterInstance sBasicServer;
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
    tsZCL_ClusterInstance sIdentifyServer;
#endif

    /* Optional Clusters Common to All HA Server Devices */
#if (defined CLD_POWER_CONFIGURATION) && (defined POWER_CONFIGURATION_SERVER)
    tsZCL_ClusterInstance sPowerConfigurationServer;
#endif

#if (defined CLD_DEVICE_TEMPERATURE_CONFIGURATION) && (defined DEVICE_TEMPERATURE_CONFIGURATION_SERVER)
    tsZCL_ClusterInstance sDeviceTemperatureConfigurationServer;
#endif

#if (defined CLD_ALARMS) && (defined ALARMS_SERVER)
    tsZCL_ClusterInstance sAlarmsServer;
#endif

#if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_SERVER)
    tsZCL_ClusterInstance sPollControlServer;
#endif

#if (defined CLD_SCENES) && (defined SCENES_SERVER)
    tsZCL_ClusterInstance sScenesServer;
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
    tsZCL_ClusterInstance sGroupsServer;
#endif

    /* Optional Clusters Common to All HA Client Devices */
#if (defined CLD_SCENES) && (defined SCENES_CLIENT)
    tsZCL_ClusterInstance sScenesClient;
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
    tsZCL_ClusterInstance sGroupsClient;
#endif
    
    /* Mandatory Clusters supported by Thermostat Server Device */
#if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_SERVER)
    tsZCL_ClusterInstance sThermostatServer;
#endif

    /* Optional Clusters supported by Thermostat Server Device */
#if (defined CLD_THERMOSTAT_UI_CONFIG) && (defined THERMOSTAT_UI_CONFIG_SERVER)
    tsZCL_ClusterInstance sThermostatUIConfigurationServer;
#endif

#if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_SERVER)
    tsZCL_ClusterInstance sTemperatureMeasurementServer;
#endif

#if (defined CLD_OCCUPANCY_SENSING) && (defined OCCUPANCY_SENSING_SERVER)
    tsZCL_ClusterInstance sOccupancySensingServer;
#endif

#if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_SERVER)
    tsZCL_ClusterInstance sRelativeHumidityMeasurementServer;
#endif

    /* Optional Clusters supported by Thermostat Client Device */
#if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_CLIENT)
    tsZCL_ClusterInstance sTemperatureMeasurementClient;
#endif

#if (defined CLD_OCCUPANCY_SENSING) && (defined OCCUPANCY_SENSING_CLIENT)
    tsZCL_ClusterInstance sOccupancySensingClient;
#endif

#if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_CLIENT)
    tsZCL_ClusterInstance sRelativeHumidityMeasurementClient;
#endif    

#ifdef CLD_OTA
    #ifdef OTA_CLIENT
    /* Add  cluster instance for the OTA cluster */
    tsZCL_ClusterInstance sOTAClient;
    #endif
    
    #ifdef OTA_SERVER
    /* Add  server instance for the OTA cluster */
    tsZCL_ClusterInstance sOTAServer;    
    #endif
#endif

#if (defined CLD_TIME) && (defined TIME_CLIENT)
    tsZCL_ClusterInstance sTimeClient;
#endif

    /* Optional client clusters */
#if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_CLIENT)
    tsZCL_ClusterInstance sThermostatClient;
#endif
    
#if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
    tsZCL_ClusterInstance sIdentifyClient;
#endif

#if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_CLIENT)
    tsZCL_ClusterInstance sPollControlClient;
#endif


#if (defined CLD_ELECTRICAL_MEASUREMENT && defined ELECTRICAL_MEASUREMENT_SERVER)
    tsZCL_ClusterInstance sElectricalMeasurementServer;
#endif

#if (defined CLD_FAN_CONTROL && defined FAN_CONTROL_SERVER)
    tsZCL_ClusterInstance sFanControlServer;
#endif

#if (defined DIAGNOSTICS_SERVER)
    tsZCL_ClusterInstance sDiagnosticServer;
#endif

} tsHA_ThermostatDeviceClusterInstances;


/* Holds everything required to create an instance of a thermostat device */
typedef struct
{
    tsZCL_EndPointDefinition sEndPoint;

    /* Cluster instances */
    tsHA_ThermostatDeviceClusterInstances sClusterInstance;

    /* All HA devices have 2 mandatory clusters - Basic(server) and Identify(server) */
#if (defined CLD_BASIC) && (defined BASIC_SERVER)
    /* Basic Cluster - Server */
    tsCLD_Basic sBasicServerCluster;
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
    /* Identify Cluster - Server */
    tsCLD_Identify sIdentifyServerCluster;
    tsCLD_IdentifyCustomDataStructure sIdentifyServerCustomDataStructure;
#endif

    /* Optional Clusters Common to All HA Server Devices */
#if (defined CLD_POWER_CONFIGURATION) && (defined POWER_CONFIGURATION_SERVER)
    /* Power Configuration Cluster - Server */
    tsCLD_PowerConfiguration sPowerConfigServerCluster;
#endif

#if (defined CLD_DEVICE_TEMPERATURE_CONFIGURATION) && (defined DEVICE_TEMPERATURE_CONFIGURATION_SERVER)
    /* Device Temperature Configuration Cluster - Server */
    tsCLD_DeviceTemperatureConfiguration sDeviceTemperatureConfigurationServerCluster;
#endif

#if (defined CLD_ALARMS) && (defined ALARMS_SERVER)
    /* Alarms Cluster - Server */
    tsCLD_Alarms sAlarmsServerCluster;
    tsCLD_AlarmsCustomDataStructure sAlarmsServerCustomDataStructure;
#endif

#if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_SERVER)
    tsCLD_PollControl sPollControlServerCluster;
    tsCLD_PollControlCustomDataStructure sPollControlServerCustomDataStructure;
#endif

#if (defined CLD_SCENES) && (defined SCENES_SERVER)
    /* Scenes Cluster - Server */
    tsCLD_Scenes sScenesServerCluster;
    tsCLD_ScenesCustomDataStructure sScenesServerCustomDataStructure;
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
    /* Groups Cluster - Server */
    tsCLD_Groups sGroupsServerCluster;
    tsCLD_GroupsCustomDataStructure sGroupsServerCustomDataStructure;
#endif

    /* Optional Clusters Common to All HA Client Devices */
#if (defined CLD_SCENES) && (defined SCENES_CLIENT)
    /* Scenes Cluster - Client */
    tsCLD_Scenes sScenesClientCluster;
    tsCLD_ScenesCustomDataStructure sScenesClientCustomDataStructure;
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
    /* Groups Cluster - Client */
    tsCLD_Groups sGroupsClientCluster;
    tsCLD_GroupsCustomDataStructure sGroupsClientCustomDataStructure;
#endif

    /* Mandatory Clusters supported by Thermostat Server Device */    
#if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_SERVER)
    tsCLD_Thermostat sThermostatServerCluster;
    tsCLD_ThermostatCustomDataStructure sThermostatServerCustomDataStructure;
#endif

    /* Optional Clusters supported by Thermostat Server Device */
#if (defined CLD_THERMOSTAT_UI_CONFIG) && (defined THERMOSTAT_UI_CONFIG_SERVER)
    tsCLD_ThermostatUIConfig sThermostatUIConfigServerCluster;
#endif

#if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_SERVER)
    tsCLD_TemperatureMeasurement sTemperatureMeasurementServerCluster;
#endif

#if (defined CLD_OCCUPANCY_SENSING) && (defined OCCUPANCY_SENSING_SERVER)
    /* Occupancy Sensing Cluster - Server */
    tsCLD_OccupancySensing sOccupancySensingServerCluster;
#endif

#if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_SERVER)
    tsCLD_RelativeHumidityMeasurement sRelativeHumidityMeasurementServerCluster;
#endif

#if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_CLIENT)
    tsCLD_TemperatureMeasurement sTemperatureMeasurementClientCluster;
#endif

#if (defined CLD_OCCUPANCY_SENSING) && (defined OCCUPANCY_SENSING_CLIENT)
    /* Occupancy Sensing Cluster - Client */
    tsCLD_OccupancySensing sOccupancySensingClientCluster;
#endif

#if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_CLIENT)
    tsCLD_RelativeHumidityMeasurement sRelativeHumidityMeasurementClientCluster;
#endif


    /* Optional Clusters supported by Thermostat Client Device */
#ifdef CLD_OTA
    /* OTA cluster */
    #ifdef OTA_CLIENT
        tsCLD_AS_Ota sCLD_OTA;
        tsOTA_Common sCLD_OTA_CustomDataStruct;
    #endif
    
    #ifdef OTA_SERVER
        tsCLD_AS_Ota sCLD_ServerOTA;     
        tsOTA_Common sCLD_OTA_ServerCustomDataStruct;
    #endif
#endif

    /* Optional client clusters */
#if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_CLIENT)
    tsCLD_Thermostat sThermostatClientCluster;
    tsCLD_ThermostatCustomDataStructure sThermostatClientCustomDataStructure;
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
    /* Identify Cluster - Client */
     tsCLD_Identify sIdentifyClientCluster;
     tsCLD_IdentifyCustomDataStructure sIdentifyClientCustomDataStructure;
#endif

#if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_CLIENT)
    tsCLD_PollControl sPollControlClientCluster;
    tsCLD_PollControlCustomDataStructure sPollControlClientCustomDataStructure;
#endif
#if (defined CLD_ELECTRICAL_MEASUREMENT && defined ELECTRICAL_MEASUREMENT_SERVER)
        tsCLD_ElectricalMeasurement sCLD_ElectricalMeasurement;
#endif

#if (defined CLD_TIME) && (defined TIME_CLIENT)
    tsCLD_Time sTimeClientCluster;
#endif
#if (defined CLD_FAN_CONTROL && defined FAN_CONTROL_SERVER)
        tsCLD_FanControl sCLD_FanControl;
#endif
#if (defined DIAGNOSTICS_SERVER)
        tsCLD_Diagnostics sCLD_Diagnostic;
#endif

} tsHA_ThermostatDevice;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eHA_RegisterThermostatEndPoint(uint8 u8EndPointIdentifier,
                                                   tfpZCL_ZCLCallBackFunction cbCallBack,
                                                   tsHA_ThermostatDevice *psDeviceInfo);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* THERMOSTAT_DEVICE_H */
