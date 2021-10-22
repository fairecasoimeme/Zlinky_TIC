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
 * MODULE:             Home Gateway
 *
 * COMPONENT:          home_gateway.h
 *
 * DESCRIPTION:        Header for ZigBee Home Gateway functions
 *
 *****************************************************************************/

#ifndef HOME_GATEWAY_H
#define HOME_GATEWAY_H

#if defined __cplusplus
extern "C" {
#endif

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "Basic.h"
#include "Identify.h"
#include "PowerConfiguration.h"
#include "DeviceTemperatureConfiguration.h"
#include "Alarms.h"

#include "ApplianceControl.h"
#include "ApplianceEventsAndAlerts.h"
#include "ApplianceIdentification.h"
#include "ApplianceStatistics.h"
#include "PowerProfile.h"

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
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define APPLIANCE_MANAGEMENT_HOME_GATEWAY_DEVICE_ID                     0x0050
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

    /* Mandatory E@H clusters */
#if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_CLIENT)
    tsZCL_ClusterInstance sASCClient;
#endif

#if (defined CLD_PP) && (defined PP_CLIENT)
    tsZCL_ClusterInstance sPPClient;
#endif

#if (defined CLD_APPLIANCE_CONTROL) && (defined APPLIANCE_CONTROL_CLIENT)
    tsZCL_ClusterInstance sACCClient;
#endif

#if (defined CLD_APPLIANCE_IDENTIFICATION) && (defined APPLIANCE_IDENTIFICATION_CLIENT)
    tsZCL_ClusterInstance sAICClient;
#endif

#if (defined CLD_APPLIANCE_EVENTS_AND_ALERTS) && (defined APPLIANCE_EVENTS_AND_ALERTS_CLIENT)
    tsZCL_ClusterInstance sAEACClient;
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

#ifdef CLD_TIME
    #ifdef TIME_SERVER
        tsZCL_ClusterInstance sTimeServer;
    #endif
    #ifdef TIME_CLIENT
        tsZCL_ClusterInstance sTimeClient;
    #endif
#endif

    /* Optional client clusters */

#if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_CLIENT)
    tsZCL_ClusterInstance sPollControlClient;
#endif

#if (defined CLD_ELECTRICAL_MEASUREMENT && defined ELECTRICAL_MEASUREMENT_SERVER)
    tsZCL_ClusterInstance sElectricalMeasurementServer;
#endif

} tsHA_HomeGatewayDeviceClusterInstances __attribute__ ((aligned(4)));


/* Holds everything required to create an instance home gateway  */
typedef struct
{
    tsZCL_EndPointDefinition sEndPoint;

    /* Cluster instances */
    tsHA_HomeGatewayDeviceClusterInstances sClusterInstance;

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

    /* Mandatory client clusters */
#if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_CLIENT)
    /* Appliance statistics - Client */
    tsCLD_ApplianceStatistics sASCClientCluster;
    tsCLD_ApplianceStatisticsCustomDataStructure sASCClientCustomDataStructure;
#endif

#if (defined CLD_PP) && (defined PP_CLIENT)
     tsCLD_PP sPPClientCluster;
     tsCLD_PPCustomDataStructure sPPClientCustomDataStructure;
#endif

#if (defined CLD_APPLIANCE_CONTROL) && (defined APPLIANCE_CONTROL_CLIENT)
    tsCLD_ApplianceControl sACCClientCluster;
    tsCLD_ApplianceControlCustomDataStructure sACCClientCustomDataStructure;
#endif

#if (defined CLD_TIME) && (defined TIME_SERVER)
    tsCLD_Time sTimeCluster;
#endif

    /* Optional client clusters */

#if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_CLIENT)
    tsCLD_PollControl sPollControlClientCluster;
    tsCLD_PollControlCustomDataStructure sPollControlClientCustomDataStructure;
#endif

#if (defined CLD_APPLIANCE_IDENTIFICATION) && (defined APPLIANCE_IDENTIFICATION_CLIENT)
    tsCLD_ApplianceIdentification sAICClientCluster;
#endif

#if (defined CLD_APPLIANCE_EVENTS_AND_ALERTS) && (defined APPLIANCE_EVENTS_AND_ALERTS_CLIENT)
    tsCLD_ApplianceEventsAndAlerts sAEACClientCluster;
    tsCLD_ApplianceEventsAndAlertsCustomDataStructure sAEACClientClusterCustomDataStructure;
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

} tsHA_HomeGatewayDevice;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eHA_RegisterHomeGatewayEndPoint(uint8 u8EndPointIdentifier,
                                              tfpZCL_ZCLCallBackFunction cbCallBack,
                                              tsHA_HomeGatewayDevice *psDeviceInfo);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* HOME_GATEWAY_H */
