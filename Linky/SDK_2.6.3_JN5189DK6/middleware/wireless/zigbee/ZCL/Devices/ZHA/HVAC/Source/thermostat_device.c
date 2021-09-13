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
 * COMPONENT:          thermostat_device.c
 *
 * DESCRIPTION:        ZigBee Thermostat device profile functions
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zps_apl.h"
#include "zcl_heap.h"
#include "thermostat_device.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: eHA_RegisterThermostatEndPoint
 *
 * DESCRIPTION:
 * Registers a Thermostat device with the ZCL layer
 *
 * PARAMETERS:  Name                            Usage
 *              u8EndPointIdentifier            Endpoint being registered
 *              cbCallBack                      Pointer to endpoint callback
 *              psDeviceInfo                    Pointer to struct containing
 *                                              data for endpoint
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eHA_RegisterThermostatEndPoint(uint8 u8EndPointIdentifier,
                                                   tfpZCL_ZCLCallBackFunction cbCallBack,
                                                   tsHA_ThermostatDevice *psDeviceInfo)
{

    /* Fill in end point details */
    psDeviceInfo->sEndPoint.u8EndPointNumber = u8EndPointIdentifier;
    psDeviceInfo->sEndPoint.u16ManufacturerCode = ZCL_MANUFACTURER_CODE;
    psDeviceInfo->sEndPoint.u16ProfileEnum = HA_PROFILE_ID;
    psDeviceInfo->sEndPoint.bIsManufacturerSpecificProfile = FALSE;
    psDeviceInfo->sEndPoint.u16NumberOfClusters = sizeof(tsHA_ThermostatDeviceClusterInstances) / sizeof(tsZCL_ClusterInstance);
    psDeviceInfo->sEndPoint.psClusterInstance = (tsZCL_ClusterInstance*)&psDeviceInfo->sClusterInstance;
    psDeviceInfo->sEndPoint.bDisableDefaultResponse = ZCL_DISABLE_DEFAULT_RESPONSES;
    psDeviceInfo->sEndPoint.pCallBackFunctions = cbCallBack;

    /* All HA devices have 2 mandatory clusters - Basic(server) and Identify(server) */
#if (defined CLD_BASIC) && (defined BASIC_SERVER)
    /* Create an instance of a Basic cluster as a server */
    if(eCLD_BasicCreateBasic(&psDeviceInfo->sClusterInstance.sBasicServer,
                          TRUE,
                          &sCLD_Basic,
                          &psDeviceInfo->sBasicServerCluster,
                          &au8BasicClusterAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_SERVER)
    /* Create an instance of an Identify cluster as a server */
    if(eCLD_IdentifyCreateIdentify(&psDeviceInfo->sClusterInstance.sIdentifyServer,
                          TRUE,
                          &sCLD_Identify,
                          &psDeviceInfo->sIdentifyServerCluster,
                          &au8IdentifyAttributeControlBits[0],
                          &psDeviceInfo->sIdentifyServerCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

    /* Optional Clusters Common to All HA Server Devices */
#if (defined CLD_POWER_CONFIGURATION) && (defined POWER_CONFIGURATION_SERVER)
    /* Create an instance of a Power Configuration cluster as a server */
    if(eCLD_PowerConfigurationCreatePowerConfiguration(&psDeviceInfo->sClusterInstance.sPowerConfigurationServer,
                          TRUE,
                          &sCLD_PowerConfiguration,
                          &psDeviceInfo->sPowerConfigServerCluster,
                          &au8PowerConfigurationAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_DEVICE_TEMPERATURE_CONFIGURATION) && (defined DEVICE_TEMPERATURE_CONFIGURATION_SERVER)
    /* Create an instance of a Device Temperature Configuration cluster as a server */
    if(eCLD_DeviceTemperatureConfigurationCreateDeviceTemperatureConfiguration(&psDeviceInfo->sClusterInstance.sDeviceTemperatureConfigurationServer,
                          TRUE,
                          &sCLD_DeviceTemperatureConfiguration,
                          &psDeviceInfo->sDeviceTemperatureConfigurationServerCluster,
                          &au8DeviceTempConfigClusterAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_ALARMS) && (defined ALARMS_SERVER)
    /* Create an instance of an Alarms cluster as a server */
    if(eCLD_AlarmsCreateAlarms(&psDeviceInfo->sClusterInstance.sAlarmsServer,
                          TRUE,
                          &sCLD_Alarms,
                          &psDeviceInfo->sAlarmsServerCluster,
                          &au8AlarmsAttributeControlBits[0],
                          &psDeviceInfo->sAlarmsServerCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_SERVER)
    /* Create an instance of a Poll Control cluster as a server */
    if(eCLD_PollControlCreatePollControl(
                          &psDeviceInfo->sClusterInstance.sPollControlServer,
                          TRUE,
                          &sCLD_PollControl,
                          &psDeviceInfo->sPollControlServerCluster,
                          &au8PollControlAttributeControlBits[0],
                          &psDeviceInfo->sPollControlServerCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif
    
#if (defined CLD_SCENES) && (defined SCENES_SERVER)
    /* Create an instance of a Scenes cluster as a server */
    if(eCLD_ScenesCreateScenes(&psDeviceInfo->sClusterInstance.sScenesServer,
                          TRUE,
                          &sCLD_Scenes,
                          &psDeviceInfo->sScenesServerCluster,
                          &au8ScenesAttributeControlBits[0],
                          &psDeviceInfo->sScenesServerCustomDataStructure,
                          &psDeviceInfo->sEndPoint) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
    /* Create an instance of a Groups cluster as a server */
    if(eCLD_GroupsCreateGroups(&psDeviceInfo->sClusterInstance.sGroupsServer,
                          TRUE,
                          &sCLD_Groups,
                          &psDeviceInfo->sGroupsServerCluster,
                          &au8GroupsAttributeControlBits[0],
                          &psDeviceInfo->sGroupsServerCustomDataStructure,
                          &psDeviceInfo->sEndPoint) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif
   
    /* Optional Clusters Common to All HA Client Devices */
#if (defined CLD_SCENES) && (defined SCENES_CLIENT)
    /* Create an instance of a Scenes cluster as a client */
    if(eCLD_ScenesCreateScenes(&psDeviceInfo->sClusterInstance.sScenesClient,
                          FALSE,
                          &sCLD_Scenes,
                          &psDeviceInfo->sScenesClientCluster,
                          &au8ScenesAttributeControlBits[0],
                          &psDeviceInfo->sScenesClientCustomDataStructure,
                          &psDeviceInfo->sEndPoint) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
    /* Create an instance of a Groups cluster as a client */
    if(eCLD_GroupsCreateGroups(&psDeviceInfo->sClusterInstance.sGroupsClient,
                          FALSE,
                          &sCLD_Groups,
                          &psDeviceInfo->sGroupsClientCluster,
                          &au8GroupsAttributeControlBits[0],
                          &psDeviceInfo->sGroupsClientCustomDataStructure,
                          &psDeviceInfo->sEndPoint) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif    
    
    /* Mandatory Clusters supported by Thermostat Server Device */
#if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_SERVER)
    /* Create an instance of a Thermostat cluster as a server */
    if(eCLD_ThermostatCreateThermostat(&psDeviceInfo->sClusterInstance.sThermostatServer,
                          TRUE,
                          &sCLD_Thermostat,
                          &psDeviceInfo->sThermostatServerCluster,
                          &au8ThermostatAttributeControlBits[0],
                          &psDeviceInfo->sThermostatServerCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

    /* Optional Clusters supported by Thermostat Server Device */
#if (defined CLD_THERMOSTAT_UI_CONFIG) && (defined THERMOSTAT_UI_CONFIG_SERVER)
    /* Create an instance of a Thermostat UI Config cluster as a server */
    if(eCLD_ThermostatUIConfigCreateThermostatUIConfig(&psDeviceInfo->sClusterInstance.sThermostatUIConfigurationServer,
                          TRUE,
                          &sCLD_ThermostatUIConfig,
                          &psDeviceInfo->sThermostatUIConfigServerCluster,
                          &au8ThermostatUIConfigAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_SERVER)
    /* Create an instance of a Temperature Measurement cluster as a server */
    if(eCLD_TemperatureMeasurementCreateTemperatureMeasurement(
                          &psDeviceInfo->sClusterInstance.sTemperatureMeasurementServer,
                          TRUE,
                          &sCLD_TemperatureMeasurement,
                          &psDeviceInfo->sTemperatureMeasurementServerCluster,
                          &au8TemperatureMeasurementAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_OCCUPANCY_SENSING) && (defined OCCUPANCY_SENSING_SERVER)
    /* Create an instance of an Occupancy Sensing cluster as a server */
    if(eCLD_OccupancySensingCreateOccupancySensing(
                          &psDeviceInfo->sClusterInstance.sOccupancySensingServer,
                          TRUE,
                          &sCLD_OccupancySensing,
                          &psDeviceInfo->sOccupancySensingServerCluster,
                          &au8OccupancySensingAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_SERVER)
    /* Create an instance of a Relative Humidity Measurement cluster as a server */
    if(eCLD_RelativeHumidityMeasurementCreateRelativeHumidityMeasurement(
                          &psDeviceInfo->sClusterInstance.sRelativeHumidityMeasurementServer,
                          TRUE,
                          &sCLD_RelativeHumidityMeasurement,
                          &psDeviceInfo->sRelativeHumidityMeasurementServerCluster,
                          &au8RelativeHumidityMeasurementAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

    /* Optional Clusters supported by Thermostat Client Device */
#if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_CLIENT)
    /* Create an instance of a Temperature Measurement cluster as a client */
    if(eCLD_TemperatureMeasurementCreateTemperatureMeasurement(
                          &psDeviceInfo->sClusterInstance.sTemperatureMeasurementClient,
                          FALSE,
                          &sCLD_TemperatureMeasurement,
                          &psDeviceInfo->sTemperatureMeasurementClientCluster,
                          &au8TemperatureMeasurementAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_OCCUPANCY_SENSING) && (defined OCCUPANCY_SENSING_CLIENT)
    /* Create an instance of an Occupancy Sensing cluster as a client */
    if(eCLD_OccupancySensingCreateOccupancySensing(
                          &psDeviceInfo->sClusterInstance.sOccupancySensingClient,
                          FALSE,
                          &sCLD_OccupancySensing,
                          &psDeviceInfo->sOccupancySensingClientCluster,
                          &au8OccupancySensingAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_CLIENT)
    /* Create an instance of a Relative Humidity Measurement cluster as a client */
    if(eCLD_RelativeHumidityMeasurementCreateRelativeHumidityMeasurement(
                          &psDeviceInfo->sClusterInstance.sRelativeHumidityMeasurementClient,
                          FALSE,
                          &sCLD_RelativeHumidityMeasurement,
                          &psDeviceInfo->sRelativeHumidityMeasurementClientCluster,
                          &au8RelativeHumidityMeasurementAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#ifdef CLD_OTA

    #ifdef OTA_CLIENT
   if (eOTA_Create(
       &psDeviceInfo->sClusterInstance.sOTAClient,
       FALSE,  /* client */
       &sCLD_OTA,
       &psDeviceInfo->sCLD_OTA,  /* cluster definition */
       u8EndPointIdentifier,
       NULL,
       &psDeviceInfo->sCLD_OTA_CustomDataStruct
       )!= E_ZCL_SUCCESS)

    {
        // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
        return E_ZCL_FAIL;
    }
    #endif
    
    #ifdef OTA_SERVER
   if (eOTA_Create(
       &psDeviceInfo->sClusterInstance.sOTAServer,
       TRUE,  /* Server */
       &sCLD_OTA,
       &psDeviceInfo->sCLD_ServerOTA,  /* cluster definition */
       u8EndPointIdentifier,
       NULL,
       &psDeviceInfo->sCLD_OTA_ServerCustomDataStruct
       )!= E_ZCL_SUCCESS)

    {
        // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
        return E_ZCL_FAIL;
    }
    #endif 
    
#endif

#if (defined CLD_TIME && defined TIME_CLIENT)
    if (eCLD_TimeCreateTime(
                    &psDeviceInfo->sClusterInstance.sTimeClient,
                    FALSE,
                    &sCLD_Time,
                    &psDeviceInfo->sTimeClientCluster,
                    &au8TimeClusterAttributeControlBits[0]) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    }
#endif    

    /* Optional client clusters */
#if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_CLIENT)
    /* Create an instance of a Thermostat cluster as a client */
    if(eCLD_ThermostatCreateThermostat(&psDeviceInfo->sClusterInstance.sThermostatClient,
                          FALSE,
                          &sCLD_Thermostat,
                          &psDeviceInfo->sThermostatClientCluster,
                          &au8ThermostatAttributeControlBits[0],
                          &psDeviceInfo->sThermostatClientCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
    /* Create an instance of an Identify cluster as a client */
    if(eCLD_IdentifyCreateIdentify(&psDeviceInfo->sClusterInstance.sIdentifyClient,
                          FALSE,
                          &sCLD_Identify,
                          &psDeviceInfo->sIdentifyClientCluster,
                          &au8IdentifyAttributeControlBits[0],
                          &psDeviceInfo->sIdentifyClientCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif


#if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_CLIENT)
    /* Create an instance of a Poll Control cluster as a client */
    if(eCLD_PollControlCreatePollControl(
                          &psDeviceInfo->sClusterInstance.sPollControlClient,
                          FALSE,
                          &sCLD_PollControl,
                          &psDeviceInfo->sPollControlClientCluster,
                          &au8PollControlAttributeControlBits[0],
                          &psDeviceInfo->sPollControlClientCustomDataStructure) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

#if (defined CLD_ELECTRICAL_MEASUREMENT && defined ELECTRICAL_MEASUREMENT_SERVER)
    if (eCLD_ElectricalMeasurementCreateElectricalMeasurement(
            &psDeviceInfo->sClusterInstance.sElectricalMeasurementServer,
            TRUE,
            &sCLD_ElectricalMeasurement,
            &psDeviceInfo->sCLD_ElectricalMeasurement,
            &au8ElectricalMeasurementAttributeControlBits[0])!= E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    }

#endif

#if (defined CLD_FAN_CONTROL && defined FAN_CONTROL_SERVER)
    if (eCLD_CreateFanControl(
            &psDeviceInfo->sClusterInstance.sFanControlServer,
            TRUE,
            &sCLD_FanControl,
            &psDeviceInfo->sCLD_FanControl,
			&au8FanControlClusterAttributeControlBits[0])!= E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    }

#endif

#if (defined DIAGNOSTICS_SERVER)
    if (eCLD_DiagnosticsCreateDiagnostics(
            &psDeviceInfo->sClusterInstance.sDiagnosticServer,
            TRUE,
            &sCLD_Diagnostics,
            &psDeviceInfo->sCLD_Diagnostic,
			&au8DiagnosticsClusterAttributeControlBits[0])!= E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    }

#endif

    return eZCL_Register(&psDeviceInfo->sEndPoint);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

