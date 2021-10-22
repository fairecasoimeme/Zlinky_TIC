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
 * COMPONENT:          mains_power_outlet.c
 *
 * DESCRIPTION:        ZigBee Mains Power Outlet profile functions
 *
 *****************************************************************************/
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Standard includes */
#include <jendefs.h>
#include <string.h>

/* Stack/profile includes */
#include "zps_apl.h"
#include "zcl_heap.h"
#include "zcl.h"
#include "zcl_options.h"
#include "mains_power_outlet.h"

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
 * NAME: eHA_RegisterMainsPowerOutletEndPoint
 *
 * DESCRIPTION:
 * Registers an On/Off Output device with the ZCL layer
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
PUBLIC teZCL_Status eHA_RegisterMainsPowerOutletEndPoint(uint8 u8EndPointIdentifier,
                                                         tfpZCL_ZCLCallBackFunction cbCallBack,
                                                         tsHA_MainsPowerOutletDevice *psDeviceInfo)
{
    /* Fill in end point details */
    psDeviceInfo->sEndPoint.u8EndPointNumber = u8EndPointIdentifier;
    psDeviceInfo->sEndPoint.u16ManufacturerCode = ZCL_MANUFACTURER_CODE;
    psDeviceInfo->sEndPoint.u16ProfileEnum = HA_PROFILE_ID;
    psDeviceInfo->sEndPoint.bIsManufacturerSpecificProfile = FALSE;
    psDeviceInfo->sEndPoint.u16NumberOfClusters = sizeof(tsHA_MainsPowerOutletDeviceClusterInstances) / sizeof(tsZCL_ClusterInstance);
    psDeviceInfo->sEndPoint.psClusterInstance = (tsZCL_ClusterInstance*)&psDeviceInfo->sClusterInstance;
    psDeviceInfo->sEndPoint.bDisableDefaultResponse = TRUE;
    psDeviceInfo->sEndPoint.pCallBackFunctions = cbCallBack;

    /* Mandatory server clusters */
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

    #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
        /* Create an instance of a On/Off cluster as a server */
        if(eCLD_OnOffCreateOnOff(&psDeviceInfo->sClusterInstance.sOnOffServer,
                              TRUE,
                              &sCLD_OnOff,
                              &psDeviceInfo->sOnOffServerCluster,
                              &au8OnOffAttributeControlBits[0],
                              &psDeviceInfo->sOnOffServerCustomDataStructure) != E_ZCL_SUCCESS)
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

    /* Optional server clusters */
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
        if(eCLD_PollControlCreatePollControl(&psDeviceInfo->sClusterInstance.sPollControlServer,
                                          TRUE,
                                          &sCLD_PollControl,
                                          &psDeviceInfo->sPollControlServerCluster,
                                          &au8PollControlAttributeControlBits[0],
                                          &psDeviceInfo->sPollControlServerCustomDataStructure) != E_ZCL_SUCCESS)
        {
            return E_ZCL_FAIL;
        } 
    #endif

    #if (defined CLD_OTA) && (defined OTA_SERVER)
        if (eOTA_Create(&psDeviceInfo->sClusterInstance.sOTAServer,
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

    /* Optional client clusters */
    #if (defined CLD_POLL_CONTROL) && (defined POLL_CONTROL_CLIENT)
        /* Create an instance of a Poll Control cluster as a client */
        if(eCLD_PollControlCreatePollControl(&psDeviceInfo->sClusterInstance.sPollControlClient,
                                          FALSE,
                                          &sCLD_PollControl,
                                          &psDeviceInfo->sPollControlClientCluster,
                                          &au8PollControlAttributeControlBits[0],
                                          &psDeviceInfo->sPollControlClientCustomDataStructure) != E_ZCL_SUCCESS)
        {
            return E_ZCL_FAIL;
        } 
    #endif

    #if (defined CLD_OTA) && (defined OTA_CLIENT)
        if (eOTA_Create(&psDeviceInfo->sClusterInstance.sOTAClient,
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

    #if (defined CLD_TIME && defined TIME_CLIENT)
       if (eCLD_TimeCreateTime(
                    &psDeviceInfo->sClusterInstance.sTimeClient,
                    FALSE,
                    &sCLD_Time,
                    &psDeviceInfo->sTimeCluster,
                    &au8TimeClusterAttributeControlBits[0]) != E_ZCL_SUCCESS)
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
