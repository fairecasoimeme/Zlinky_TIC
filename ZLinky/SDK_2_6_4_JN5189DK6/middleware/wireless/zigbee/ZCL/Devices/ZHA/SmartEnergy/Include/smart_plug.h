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
 * MODULE:             SMART PLUG
 *
 * COMPONENT:          smart_plug.h
 *
 * DESCRIPTION:        Header for ZigBee smart plug device
 *
 *****************************************************************************/

#ifndef SMART_PLUG_H
#define SMART_PLUG_H

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
#ifdef CLD_SIMPLE_METERING
#include "SimpleMetering.h"
#endif
#ifdef CLD_ONOFF
#include "OnOff.h"
#endif
#ifdef CLD_PRICE
#include "price.h"
#endif
#ifdef CLD_DRLC
#include "drlc.h"
#endif
#ifdef CLD_ELECTRICAL_MEASUREMENT
	#include "ElectricalMeasurement.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define SMART_ENERGY_SMART_PLUG_DEVICE_ID                               0x0051
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
    
    /* Mandatory Clusters supported by Smart Plug Server Device */
#if (defined CLD_SIMPLE_METERING) && (defined SM_SERVER)
    tsZCL_ClusterInstance sSimpleMeteringServer;
#endif

#if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
    tsZCL_ClusterInstance sOnOffServer;
#endif

    /* Mandatory Clusters supported by Smart Plug Client Device */
#if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
    tsZCL_ClusterInstance sIdentifyClient;
#endif

    /* Optional Clusters supported by Smart Plug Client Device */
#if (defined CLD_DRLC) && (defined DRLC_CLIENT)
    tsZCL_ClusterInstance sDRLCClient;
#endif

#if (defined CLD_PRICE) && defined(PRICE_CLIENT)
    tsZCL_ClusterInstance sPriceClient;
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


#if (defined CLD_ELECTRICAL_MEASUREMENT && defined ELECTRICAL_MEASUREMENT_SERVER)
    tsZCL_ClusterInstance sElectricalMeasurementServer;
#endif
} tsHA_SmartPlugClusterInstances __attribute__ ((aligned(4)));


/* Holds everything required to create an instance of a thermostat device */
typedef struct
{
    tsZCL_EndPointDefinition sEndPoint;

    /* Cluster instances */
    tsHA_SmartPlugClusterInstances sClusterInstance;

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

        /* Mandatory Clusters supported by Smart Plug Server Device */
#if (defined CLD_SIMPLE_METERING) && (defined SM_SERVER)
    /* Holds the attributes for the simple metering cluster */
    tsCLD_SimpleMetering sSimpleMeteringServerCluster;
    /*Event Address, Custom call back event, Custom call back message*/
    tsSM_CustomStruct sSimpleMeteringServerCustomDataStruct;
#endif

#if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
    /* On/Off Cluster - Server */
    tsCLD_OnOff sOnOffServerCluster;
    tsCLD_OnOffCustomDataStructure     sOnOffServerCustomDataStructure;
#endif

    /* Mandatory Clusters supported by Smart Plug Client Device */
#if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
    /* Identify Cluster - Client */
    tsCLD_Identify sIdentifyClientCluster;
    tsCLD_IdentifyCustomDataStructure sIdentifyClientCustomDataStructure;
#endif

    /* Optional Clusters supported by Smart Plug Client Device */
#if (defined CLD_DRLC) && (defined DRLC_CLIENT)
    tsCLD_DRLC sDRLCClientCluster;
    tsSE_DRLCCustomDataStructure sDRLCClientCustomDataStructure;
    tsSE_DRLCLoadControlEventRecord asDRLCLoadControlEventRecord[SE_DRLC_NUMBER_OF_CLIENT_LOAD_CONTROL_ENTRIES];
#endif

#if (defined CLD_PRICE) && defined(PRICE_CLIENT)
    /* price cluster */
    tsCLD_Price sPriceClientCluster;
    /* custom data structures */
    tsSE_PriceCustomDataStructure sPriceClientCustomDataStructure;
    tsSE_PricePublishPriceRecord asPublishPriceRecord[SE_PRICE_NUMBER_OF_CLIENT_PRICE_RECORD_ENTRIES];
    uint8 au8RateLabel[SE_PRICE_NUMBER_OF_CLIENT_PRICE_RECORD_ENTRIES][SE_PRICE_SERVER_MAX_STRING_LENGTH];

#ifdef BLOCK_CHARGING
    /* Block Period */
    tsSE_PricePublishBlockPeriodRecord asPublishBlockPeriodRecord[SE_PRICE_NUMBER_OF_CLIENT_BLOCK_PERIOD_RECORD_ENTRIES];
#endif /* BLOCK_CHARGING */
#ifdef PRICE_CONVERSION_FACTOR
    tsSE_PriceConversionFactorRecord asPublishConversionFactorRecord[SE_PRICE_NUMBER_OF_CONVERSION_FACTOR_ENTRIES];
#endif

#ifdef PRICE_CALORIFIC_VALUE
    tsSE_PriceCalorificValueRecord   asPublishCalorificValueRecord[SE_PRICE_NUMBER_OF_CALORIFIC_VALUE_ENTRIES];
#endif
#endif

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

#if (defined CLD_ELECTRICAL_MEASUREMENT && defined ELECTRICAL_MEASUREMENT_SERVER)
        tsCLD_ElectricalMeasurement sCLD_ElectricalMeasurement;
#endif

#if (defined CLD_TIME) && (defined TIME_CLIENT)
    tsCLD_Time sTimeClientCluster;
#endif

} tsHA_SmartPlugDevice;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eHA_RegisterSmartPlugEndPoint(uint8 u8EndPointIdentifier,
                                                   tfpZCL_ZCLCallBackFunction cbCallBack,
                                                   tsHA_SmartPlugDevice *psDeviceInfo);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* SMART_PLUG_H */
