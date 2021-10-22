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
 * MODULE:             SimpleMetering Sensor
 *
 * COMPONENT:          metering_sensor.h
 *
 * DESCRIPTION:        Header for ZigBee Simple Metering sensor profile functions
 *
 *****************************************************************************/

#ifndef METERING_SENSOR_H
#define METERING_SENSOR_H

#if defined __cplusplus
extern "C" {
#endif

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "Basic.h"
#include "Identify.h"
#include "Groups.h"

#ifdef CLD_OTA
#include "OTA.h"
#endif
#ifdef CLD_ALARMS
#include "Alarms.h"
#endif
#include "PollControl.h"
#include "SimpleMetering.h"
#include "ElectricalMeasurement.h"
#include "LixeeCluster.h"
#include "meter_identification.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Holds cluster instances */
typedef struct 
{
    /* All ZLO devices have 2 mandatory clusters - Basic(server) and Identify(server) */
    #if (defined CLD_BASIC) && (defined BASIC_SERVER)
        tsZCL_ClusterInstance sBasicServer;
    #endif

    #if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
        tsZCL_ClusterInstance sIdentifyServer;
    #endif

	#if (defined CLD_ALARMS) && (defined ALARMS_SERVER)
		tsZCL_ClusterInstance sAlarmsServer;
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

    /* Recommended Optional client clusters */
    #if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
        tsZCL_ClusterInstance sGroupsClient;
    #endif

    #if (defined CLD_OTA) && (defined OTA_CLIENT)
        /* Add  cluster instance for the OTA cluster */
        tsZCL_ClusterInstance sOTAClient;
    #endif

    tsZCL_ClusterInstance sLinkyServer;

    /* Optional cluster not part of ZLO */

} tsZLO_LinkyDeviceClusterInstances __attribute__ ((aligned(4)));


/* Holds everything required to create an instance of a Light Sensor */
typedef struct
{
    tsZCL_EndPointDefinition sEndPoint;

    /* Cluster instances */
    tsZLO_LinkyDeviceClusterInstances sClusterInstance;

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

	#if (defined CLD_ALARMS) && (defined ALARMS_SERVER)
		/* Alarms Cluster - Server */
		tsCLD_Alarms sAlarmsServerCluster;
		tsCLD_AlarmsCustomDataStructure sAlarmsServerCustomDataStructure;
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

    /* Optional server clusters */
    #if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_SERVER)
        tsCLD_PollControl sPollControlServerCluster;
        tsCLD_PollControlCustomDataStructure sPollControlServerCustomDataStructure;
    #endif

    /* Recommended Optional client clusters */
    #if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
        /* Groups Cluster - Client */
        tsCLD_Groups sGroupsClientCluster;
        tsCLD_GroupsCustomDataStructure sGroupsClientCustomDataStructure;
    #endif

    #if (defined CLD_OTA) && (defined OTA_CLIENT)
        tsCLD_AS_Ota sCLD_OTA;
        tsOTA_Common sCLD_OTA_CustomDataStruct;
    #endif

    tsCLD_Linky sLinkyServerCluster;

} tsZLO_LinkyDevice;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eZLO_RegisterSimpleMeteringEndPoint(uint8 u8EndPointIdentifier,
                                              tfpZCL_ZCLCallBackFunction cbCallBack,
                                              tsZLO_LinkyDevice *psDeviceInfo);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* METERING_SENSOR_H */
