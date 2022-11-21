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
 * MODULE:             Base Device Template
 *
 * COMPONENT:          base_device.h
 *
 * DESCRIPTION:        Header for ZigBee Base Device Template
 *
 *****************************************************************************/

#ifndef BASE_DEVICE_H
#define BASE_DEVICE_H

#if defined __cplusplus
extern "C" {
#endif

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "Basic.h"
#include "Identify.h"
#include "Groups.h"
#include "OnOff.h"
#ifdef CLD_OTA
#include "OTA.h"
#endif

#include "SimpleMetering.h"
#include "LixeeCluster.h"
#include "ElectricalMeasurement.h"
#include "meter_identification.h"
#include "PowerConfiguration.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define GENERIC_BASE_DEVICE_ID                                 0xf002	// todo
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Holds cluster instances */
typedef struct 
{
    /* All ) */
#if (defined CLD_BASIC) && (defined BASIC_SERVER)
    tsZCL_ClusterInstance sBasicServer;
#endif

#if (defined CLD_BASIC) && (defined BASIC_CLIENT)
    tsZCL_ClusterInstance sBasicClient;
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
    tsZCL_ClusterInstance sIdentifyServer;
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
    tsZCL_ClusterInstance sIdentifyClient;
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
    tsZCL_ClusterInstance sGroupsServer;
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
    tsZCL_ClusterInstance sGroupsClient;
#endif

#if (defined CLD_SIMPLE_METERING) && (defined SM_SERVER)
   tsZCL_ClusterInstance sSimpleMeteringServer;
#endif

#if (defined CLD_ELECTRICAL_MEASUREMENT) && (defined ELECTRICAL_MEASUREMENT_SERVER)
   tsZCL_ClusterInstance sElectricalMeasurementServer;
#endif

#if (defined CLD_METER_IDENTIFICATION) && (defined METER_IDENTIFICATION_SERVER)
   tsZCL_ClusterInstance sMeterIdentificationServer;
#endif

    /* optional cluster */
#if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
    tsZCL_ClusterInstance sOnOffServer;
#endif

#if (defined CLD_ONOFF) && (defined ONOFF_CLIENT)
    tsZCL_ClusterInstance sOnOffClient;
#endif

/* OTA Cluster Instance */
#if (defined CLD_OTA) && (defined OTA_CLIENT)
    tsZCL_ClusterInstance sOTAClient;
#endif

#if (defined CLD_POWER_CONFIGURATION) && (defined POWER_CONFIGURATION_SERVER)
    tsZCL_ClusterInstance sPowerConfigurationServer;
#endif

#if (defined CLD_OTA) && (defined OTA_SERVER)
    tsZCL_ClusterInstance sOTAServer;
#endif

    tsZCL_ClusterInstance sLinkyServer;

} tsZHA_BaseDeviceClusterInstances __attribute__ ((aligned(4)));


/* Holds everything required to create an instance of base device */
typedef struct
{
    tsZCL_EndPointDefinition sEndPoint;

    /* Cluster instances */
    tsZHA_BaseDeviceClusterInstances sClusterInstance;

    /* Mandatory server clusters */
#if (defined CLD_BASIC) && (defined BASIC_SERVER)
    /* Basic Cluster - Server */
    tsCLD_Basic sBasicServerCluster;
#endif

#if (defined CLD_BASIC) && (defined BASIC_CLIENT)
    /* Basic Cluster - Client */
    tsCLD_Basic sBasicClientCluster;
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
    /* Identify Cluster - Server */
    tsCLD_Identify sIdentifyServerCluster;
    tsCLD_IdentifyCustomDataStructure sIdentifyServerCustomDataStructure;
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
    /* Identify Cluster - Client */
    tsCLD_Identify sIdentifyClientCluster;
    tsCLD_IdentifyCustomDataStructure sIdentifyClientCustomDataStructure;
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
    /* Groups Cluster - Server */
    tsCLD_Groups sGroupsServerCluster;
    tsCLD_GroupsCustomDataStructure sGroupsServerCustomDataStructure;
#endif

#if (defined CLD_SIMPLE_METERING) && (defined SM_SERVER)
	/* Simple MEtering Measurement Cluster - Server */
	tsCLD_SimpleMetering sSimpleMeteringServerCluster;
	tsSM_CustomStruct sSimpleMeteringServerCustomDataStruct;
#endif

#if (defined CLD_ELECTRICAL_MEASUREMENT) && (defined ELECTRICAL_MEASUREMENT_SERVER)
	tsCLD_ElectricalMeasurement sElectricalMeasurement;
#endif

#if (defined CLD_METER_IDENTIFICATION) && (defined METER_IDENTIFICATION_SERVER)
	tsCLD_MeterIdentification sMeterIdentification;
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
    /* Groups Cluster - Client */
    tsCLD_Groups sGroupsClientCluster;
    tsCLD_GroupsCustomDataStructure sGroupsClientCustomDataStructure;
#endif

#if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
    /* On/Off Cluster - Server */
    tsCLD_OnOff sOnOffServerCluster;
    tsCLD_OnOffCustomDataStructure     sOnOffServerCustomDataStructure;
#endif

#if (defined CLD_ONOFF) && (defined ONOFF_CLIENT)
    tsCLD_OnOffClient sOnOffClientCluster;
#endif

#if (defined CLD_OTA) && (defined OTA_CLIENT)
     /* OTA cluster - Client */
     tsCLD_AS_Ota sCLD_OTA;
     tsOTA_Common sCLD_OTA_CustomDataStruct;
#endif

#if (defined CLD_POWER_CONFIGURATION) && (defined POWER_CONFIGURATION_SERVER)
    /* Power Configuration Cluster - Server */
    tsCLD_PowerConfiguration sPowerConfigServerCluster;
#endif

#if (defined CLD_OTA) && (defined OTA_SERVER)
     /* OTA cluster - Server */
     tsCLD_AS_Ota sCLD_OTAServer;
     tsOTA_Common sCLD_OTA_CustomDataStruct;
#endif

     tsCLD_Linky sLinkyServerCluster;


} tsZHA_BaseDevice;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eZHA_RegisterBaseDeviceEndPoint(uint8 u8EndPointIdentifier,
                                              tfpZCL_ZCLCallBackFunction cbCallBack,
                                              tsZHA_BaseDevice *psDeviceInfo);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* BASE_DEVICE_H */
