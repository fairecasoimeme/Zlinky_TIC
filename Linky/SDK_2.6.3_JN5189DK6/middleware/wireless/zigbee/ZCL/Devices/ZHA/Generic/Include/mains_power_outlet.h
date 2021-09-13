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
 * MODULE:             Mains Power Outlet
 *
 * COMPONENT:          mains_power_outlet.h
 *
 * DESCRIPTION:        Header for ZigBee mains power outlet profile functions
 *
 *****************************************************************************/
#ifndef MAINS_POWER_OUTLET_H
#define MAINS_POWER_OUTLET_H

#if defined __cplusplus
extern "C" {
#endif

/* Standard includes */
#include <jendefs.h>

/* Stack/profile includes */
#include "zcl.h"
#include "zcl_options.h"
#include "Basic.h"
#include "Identify.h"
#include "OnOff.h"
#include "Scenes.h"
#include "Groups.h"

#ifdef CLD_POWER_CONFIGURATION
    #include "PowerConfiguration.h"
#endif

#ifdef CLD_DEVICE_TEMPERATURE_CONFIGURATION
    #include "DeviceTemperatureConfiguration.h"
#endif

#ifdef CLD_ALARMS
    #include "Alarms.h"
#endif

#ifdef CLD_POLL_CONTROL
    #include "PollControl.h"
#endif

#ifdef CLD_OTA
    #include "OTA.h"
#endif

#ifdef CLD_TIME
    #include "Time.h"
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
    /* All HA devices have 2 mandatory clusters - Basic(server) and Identify(server) */
    #if (defined CLD_BASIC) && (defined BASIC_SERVER)
        tsZCL_ClusterInstance sBasicServer;
    #endif

    #if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
        tsZCL_ClusterInstance sIdentifyServer;
    #endif

    /* Mains Power Outlet has three additional mandatory (server) clusters */
    #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
        tsZCL_ClusterInstance sOnOffServer;
    #endif

    #if (defined CLD_SCENES) && (defined SCENES_SERVER)
        tsZCL_ClusterInstance sScenesServer;
    #endif

    #if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
        tsZCL_ClusterInstance sGroupsServer;
    #endif

    /* Optional server clusters */
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

    /* Optional client clusters */
    #if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_CLIENT)
        tsZCL_ClusterInstance sPollControlClient;
    #endif

    #ifdef CLD_TIME
        tsZCL_ClusterInstance sTimeClient;
    #endif
} tsHA_MainsPowerOutletDeviceClusterInstances __attribute__ ((aligned(4)));


/* Holds everything required to create an instance of an main power outlet */
typedef struct
{
    tsZCL_EndPointDefinition sEndPoint;

    /* Cluster instances */
    tsHA_MainsPowerOutletDeviceClusterInstances sClusterInstance;

    /* Mandatory server clusters */
    #if (defined CLD_BASIC) && (defined BASIC_SERVER)
        /* Basic Cluster - Server */
        tsCLD_Basic sBasicServerCluster;
    #endif

    #if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
        /* Identify Cluster - Server */
        tsCLD_Identify sIdentifyServerCluster;
        tsCLD_IdentifyCustomDataStructure sIdentifyServerCustomDataStructure;
    #endif

    #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
        /* On/Off Cluster - Server */
        tsCLD_OnOff sOnOffServerCluster;
        tsCLD_OnOffCustomDataStructure     sOnOffServerCustomDataStructure;
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

    /* Optional server clusters */
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
    #if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_CLIENT)
        tsCLD_PollControl sPollControlClientCluster;
        tsCLD_PollControlCustomDataStructure sPollControlClientCustomDataStructure;
    #endif

    #ifdef CLD_TIME
        tsCLD_Time sTimeCluster;
    #endif

} tsHA_MainsPowerOutletDevice;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eHA_RegisterMainsPowerOutletEndPoint(uint8 u8EndPointIdentifier,
                                                         tfpZCL_ZCLCallBackFunction cbCallBack,
                                                         tsHA_MainsPowerOutletDevice *psDeviceInfo);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* MAINS_POWER_OUTLET_H */
