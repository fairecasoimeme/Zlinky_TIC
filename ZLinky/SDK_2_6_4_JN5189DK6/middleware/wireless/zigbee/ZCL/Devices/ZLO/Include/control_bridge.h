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
 * MODULE:             Zigbee Lighting and Occupancy
 *
 * COMPONENT:          control_bridge.h
 *
 * DESCRIPTION:        Header for ZLO control bridge
 *
 *****************************************************************************/

#ifndef CONTROL_BRIDGE_H
#define CONTROL_BRIDGE_H

#if defined __cplusplus
extern "C" {
#endif

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "zll_commission.h"
#include "zll_utility.h"
#include "Basic.h"
#include "Identify.h"
#include "OnOff.h"
#include "LevelControl.h"
#include "ColourControl.h"
#include "Groups.h"
#include "Scenes.h"
#include "IlluminanceLevelSensing.h"
#include "Thermostat.h"
#include "ThermostatUIConfig.h"
#include "TemperatureMeasurement.h"
#include "RelativeHumidityMeasurement.h"
#include "IlluminanceMeasurement.h"
#include "OccupancySensing.h"
#ifdef CLD_IASZONE
#include "IASZone.h"
#endif
#ifdef CLD_DOOR_LOCK
#include "DoorLock.h"
#endif
#ifdef CLD_SIMPLE_METERING
#include "SimpleMetering.h"
#endif
#ifdef CLD_OTA
#include "OTA.h"
#endif
#include "ApplianceStatistics.h"
#ifdef CLD_DIAGNOSTICS
#include "Diagnostics.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Holds cluster instances */
typedef struct
{
    /*
     * All ZLO devices have following mandatory clusters
     */

    #if (defined CLD_BASIC) && (defined BASIC_SERVER)
        tsZCL_ClusterInstance sBasicServer;
    #endif

    #if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
        tsZCL_ClusterInstance sIdentifyServer;
    #endif

    /* Recommended Optional Server Cluster*/
    #if (defined CLD_ZLL_COMMISSION) && (defined ZLL_COMMISSION_SERVER)
        tsZCL_ClusterInstance sZllCommissionServer;
    #endif

    #if (defined CLD_OTA) && (defined OTA_SERVER)
        /* Add  server instance for the OTA cluster */
        tsZCL_ClusterInstance sOTAServer;    
    #endif

    /*
     * Control Bridge Device has 7 other mandatory clusters for the client
     */
   
    #if (defined CLD_BASIC) && (defined BASIC_CLIENT)
        tsZCL_ClusterInstance sBasicClient;
    #endif
    
    #if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
        tsZCL_ClusterInstance sIdentifyClient;
    #endif

    #if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
        tsZCL_ClusterInstance sGroupsClient;
    #endif

    #if (defined CLD_SCENES) && (defined SCENES_CLIENT)
        tsZCL_ClusterInstance sScenesClient;
    #endif
    
    #if (defined CLD_ONOFF) && (defined ONOFF_CLIENT)
        tsZCL_ClusterInstance sOnOffClient;
    #endif
    
    #if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_CLIENT)
        tsZCL_ClusterInstance sLevelControlClient;
    #endif

    #if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_CLIENT)
        tsZCL_ClusterInstance sColourControlClient;
    #endif

    /* Recommend Optional client clusters */
    #if (defined CLD_OTA) && (defined OTA_CLIENT)
        tsZCL_ClusterInstance sOTAClient;
    #endif
    
    #if (defined CLD_ILLUMINANCE_MEASUREMENT) && (defined ILLUMINANCE_MEASUREMENT_CLIENT)
        tsZCL_ClusterInstance sIlluminanceMeasurementClient;
    #endif
    
    #if (defined CLD_ILLUMINANCE_LEVEL_SENSING) && (defined ILLUMINANCE_LEVEL_SENSING_CLIENT)
        tsZCL_ClusterInstance sIlluminanceLevelSensingClient;
    #endif

    #if (defined CLD_OCCUPANCY_SENSING) && (defined OCCUPANCY_SENSING_CLIENT)
        tsZCL_ClusterInstance sOccupancySensingClient;
    #endif

    #if (defined CLD_ZLL_COMMISSION) && (defined ZLL_COMMISSION_CLIENT)
        tsZCL_ClusterInstance sZllCommissionClient;
    #endif
    
    /* Extra server cluster */
    #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
        tsZCL_ClusterInstance sOnOffServer;
    #endif

    #if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
        tsZCL_ClusterInstance sGroupsServer;
    #endif

    #if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_SERVER)
        tsZCL_ClusterInstance sASCServer;
    #endif    
    
    /* Extra client cluster */
    #if (defined CLD_IASZONE) && (defined IASZONE_CLIENT)
        tsZCL_ClusterInstance sIASZoneClient;
    #endif

    #if (defined CLD_DOOR_LOCK) && (defined DOOR_LOCK_CLIENT)
        tsZCL_ClusterInstance sDoorLockClient;
    #endif
    
    #if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_CLIENT)
        tsZCL_ClusterInstance sThermostatClient;
    #endif
    
    #if (defined CLD_SIMPLE_METERING) && (defined SM_CLIENT)
        tsZCL_ClusterInstance sMeteringClient;
    #endif
    
    #if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_CLIENT)
        tsZCL_ClusterInstance sTemperatureMeasurementClient;
    #endif

    #if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_CLIENT)
        tsZCL_ClusterInstance sRelativeHumidityMeasurementClient;
    #endif

    #if (defined CLD_DIAGNOSTICS) && (defined DIAGNOSTICS_CLIENT)
        tsZCL_ClusterInstance sDiagnosticClient;
    #endif

    #if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_CLIENT)
        tsZCL_ClusterInstance sASCClient;
    #endif

} tsZLO_ControlBridgeDeviceClusterInstances __attribute__ ((aligned(4)));




/* Holds everything required to create an instance of a Control Bridge */
typedef struct
{
    tsZCL_EndPointDefinition sEndPoint;

    /* Cluster instances */
    tsZLO_ControlBridgeDeviceClusterInstances sClusterInstance;

    /* Mandatory server cluster */
    #if (defined CLD_BASIC) && (defined BASIC_SERVER)
        /* Basic Cluster - Server */
        tsCLD_Basic sBasicServerCluster;
    #endif

    #if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
        /* Identify Cluster - Server */
        tsCLD_Identify sIdentifyServerCluster;
        tsCLD_IdentifyCustomDataStructure sIdentifyServerCustomDataStructure;
    #endif

    /* Recommended Optional Server Cluster */
    
    #if (defined CLD_OTA) && (defined OTA_SERVER)
    /* OTA cluster */
        tsCLD_AS_Ota sCLD_ServerOTA;    
        tsOTA_Common sCLD_OTA_ServerCustomDataStruct;
    #endif
    
    #if (defined CLD_ZLL_COMMISSION) && (defined ZLL_COMMISSION_SERVER)
        tsCLD_ZllCommission                         sZllCommissionServerCluster;
        tsCLD_ZllCommissionCustomDataStructure      sZllCommissionServerCustomDataStructure;
    #endif
    
    /*
     * Mandatory client clusters
     */
     
    #if (defined CLD_BASIC) && (defined BASIC_CLIENT)
        /* Basic Cluster - Client */
        tsCLD_Basic sBasicClientCluster;
    #endif
    
    #if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
        /* Identify Cluster - Client */
        tsCLD_Identify sIdentifyClientCluster;
        tsCLD_IdentifyCustomDataStructure sIdentifyClientCustomDataStructure;
    #endif

    #if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
        /* Groups Cluster - Client */
        tsCLD_Groups sGroupsClientCluster;
        tsCLD_GroupsCustomDataStructure sGroupsClientCustomDataStructure;
    #endif
    
    #if (defined CLD_SCENES) && (defined SCENES_CLIENT)
        /* Scenes Cluster - Client */
        tsCLD_Scenes sScenesClientCluster;
        tsCLD_ScenesCustomDataStructure sScenesClientCustomDataStructure;
    #endif
    
    #if (defined CLD_ONOFF) && (defined ONOFF_CLIENT)
        /* On/Off Cluster - Client */
        tsCLD_OnOffClient sOnOffClientCluster;
    #endif

    #if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_CLIENT)
        /* Level Control Cluster - Client */
        tsCLD_LevelControlClient sLevelControlClientCluster;
        tsCLD_LevelControlCustomDataStructure sLevelControlClientCustomDataStructure;
    #endif

    #if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_CLIENT)
        /* Colour Control Cluster - Client */
        tsCLD_ColourControl sColourControlClientCluster;    
        tsCLD_ColourControlCustomDataStructure sColourControlClientCustomDataStructure;
    #endif

        /* Recommended Optional client clusters */
        
    #if (defined CLD_OTA) && (defined OTA_CLIENT)
        /* OTA cluster */
        tsCLD_AS_Ota sCLD_OTA;    
        tsOTA_Common sCLD_OTA_CustomDataStruct;
    #endif
    
    #if (defined CLD_ILLUMINANCE_MEASUREMENT) && (defined ILLUMINANCE_MEASUREMENT_CLIENT)
        /* Illuminance Measurement Cluster - Client */
        tsCLD_IlluminanceMeasurement sIlluminanceMeasurementClientCluster;
    #endif

    #if (defined CLD_ILLUMINANCE_LEVEL_SENSING) && (defined ILLUMINANCE_LEVEL_SENSING_CLIENT)
        tsCLD_IlluminanceLevelSensing sIlluminanceLevelSensingClientCluster;
    #endif

    #if (defined CLD_OCCUPANCY_SENSING) && (defined OCCUPANCY_SENSING_CLIENT)
        /* Occupancy Sensing Cluster - Client */
        tsCLD_OccupancySensing sOccupancySensingClientCluster;
    #endif

    #if (defined CLD_ZLL_COMMISSION) && (defined ZLL_COMMISSION_CLIENT)
        tsCLD_ZllCommission                         sZllCommissionClientCluster;
        tsCLD_ZllCommissionCustomDataStructure      sZllCommissionClientCustomDataStructure;
    #endif

    /*Optional Server cluster */
    #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
        /* On/Off Cluster - Client */
        tsCLD_OnOff sOnOffServerCluster;
        tsCLD_OnOffCustomDataStructure sOnOffServerCustomDataStructure;
    #endif

    #if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
        /* Groups Cluster - Server */
        tsCLD_Groups sGroupsServerCluster;
        tsCLD_GroupsCustomDataStructure sGroupsServerCustomDataStructure;
    #endif
    
    #if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_SERVER)
        tsZCL_ClusterInstance sASCServerCluster;
        tsCLD_ApplianceStatisticsCustomDataStructure sASCServerCustomDataStructure;
    #endif
    
    /* Optional client cluster */
    #if (defined CLD_IASZONE) && (defined IASZONE_CLIENT)
        /* IAS Zone - Client */
        tsCLD_IASZone sIASZoneClientCluster;    
        tsCLD_IASZone_CustomDataStructure sIASZoneClientCustomDataStructure;
    #endif

    #if (defined CLD_DOOR_LOCK) && (defined DOOR_LOCK_CLIENT)
        /* door lock Cluster - Client */
        tsCLD_DoorLock sDoorLockClientCluster;
    #endif
    
    #if (defined CLD_SIMPLE_METERING) && (defined SM_CLIENT)
        /* Simple Metering Cluster - Client */
        tsSM_CustomStruct sMeteringClientCustomDataStructure;
    #endif

    #if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_CLIENT)
        tsCLD_Thermostat sThermostatClientCluster;        
        tsCLD_ThermostatCustomDataStructure sThermostatClientCustomDataStructure;
    #endif
    
    #if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_CLIENT)
        tsCLD_TemperatureMeasurement sTemperatureMeasurementClientCluster;
    #endif

    #if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_CLIENT)
        tsCLD_RelativeHumidityMeasurement sRelativeHumidityMeasurementClientCluster;
    #endif

    #if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_CLIENT)
        /* Basic Cluster - Client */
        tsCLD_ApplianceStatistics sASCClientCluster;
        tsCLD_ApplianceStatisticsCustomDataStructure sASCClientCustomDataStructure;
    #endif

    #if (defined CLD_DIAGNOSTICS) && (defined DIAGNOSTICS_CLIENT)
        tsZCL_ClusterInstance sDiagnosticClientCluster;
    #endif

} tsZLO_ControlBridgeDevice;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eZLO_RegisterControlBridgeEndPoint( uint8                         u8EndPointIdentifier,
                                                        tfpZCL_ZCLCallBackFunction    cbCallBack,
                                                        tsZLO_ControlBridgeDevice*    psDeviceInfo );

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* CONTROL_BRIDGE_H */
