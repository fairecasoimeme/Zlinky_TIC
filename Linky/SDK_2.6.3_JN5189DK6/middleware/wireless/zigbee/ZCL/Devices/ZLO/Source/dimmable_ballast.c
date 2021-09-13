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
 * MODULE:             Dimmable Ballast
 *
 * COMPONENT:          dimmable_ballast.c
 *
 * DESCRIPTION:        ZigBee Dimmable Ballast profile functions
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zps_apl.h"
#include "zcl_heap.h"
#include "dimmable_ballast.h"
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
 * NAME: eZLO_RegisterDimmableBallastEndPoint
 *
 * DESCRIPTION:
 * Registers a OnOff Blast device with the ZCL layer
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
PUBLIC teZCL_Status eZLO_RegisterDimmableBallastEndPoint(uint8 u8EndPointIdentifier,
                                              tfpZCL_ZCLCallBackFunction cbCallBack,
                                              tsZLO_DimmableBallastDevice *psDeviceInfo)
{

    /* Fill in end point details */
    psDeviceInfo->sEndPoint.u8EndPointNumber = u8EndPointIdentifier;
    psDeviceInfo->sEndPoint.u16ManufacturerCode = ZCL_MANUFACTURER_CODE;
    psDeviceInfo->sEndPoint.u16ProfileEnum = HA_PROFILE_ID;
    psDeviceInfo->sEndPoint.bIsManufacturerSpecificProfile = FALSE;
    psDeviceInfo->sEndPoint.u16NumberOfClusters = sizeof(tsZLO_DimmableBallastDeviceClusterInstances) / sizeof(tsZCL_ClusterInstance);
    psDeviceInfo->sEndPoint.psClusterInstance = (tsZCL_ClusterInstance*)&psDeviceInfo->sClusterInstance;
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
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
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
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
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
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }  
    #endif
    
    #if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_SERVER)
        /* Create an instance of a Level Control cluster as a server */
        if(eCLD_LevelControlCreateLevelControl(&psDeviceInfo->sClusterInstance.sLevelControlServer,
                              TRUE,
                              &sCLD_LevelControl,
                              &psDeviceInfo->sLevelControlServerCluster,
                              &au8LevelControlAttributeControlBits[0],
                              &psDeviceInfo->sLevelControlServerCustomDataStructure) != E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
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
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
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
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        } 
    #endif

    #if (defined CLD_POWER_CONFIGURATION) && (defined POWER_CONFIGURATION_SERVER)
        /* Create an instance of a Power Configuration cluster as a server */
        if(eCLD_PowerConfigurationCreatePowerConfiguration(&psDeviceInfo->sClusterInstance.sPowerConfigurationServer,
                              TRUE,
                              &sCLD_PowerConfiguration,
                              &psDeviceInfo->sPowerConfigServerCluster,
                              &au8PowerConfigurationAttributeControlBits[0]) != E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
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
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        } 
    #endif

    #if (defined CLD_BALLAST_CONFIGURATION) && (defined BALLAST_CONFIGURATION_SERVER)
        /* Create an instance of a Ballast Configuration cluster as a server */
        if(eCLD_BallastConfigurationCreateBallastConfiguration(&psDeviceInfo->sClusterInstance.sBallastConfigurationServer,
                              TRUE,
                              &sCLD_BallastConfiguration,
                              &psDeviceInfo->sBallastConfigurationServerCluster,
                              &au8BallastConfigurationAttributeControlBits[0]) != E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        } 
    #endif

    /*Recommended Optional server clusters */
    #if (defined CLD_ILLUMINANCE_LEVEL_SENSING) && (defined ILLUMINANCE_LEVEL_SENSING_SERVER)
        /* Create an instance of an Illuminance Level Sensing  cluster as a client */
        if(eCLD_IlluminanceLevelSensingCreateIlluminanceLevelSensing(
                              &psDeviceInfo->sClusterInstance.sIlluminanceLevelSensingServer,
                              FALSE,
                              &sCLD_IlluminanceLevelSensing,
                              &psDeviceInfo->sIlluminanceLevelSensingServerCluster,
                              &au8IlluminanceLevelSensingAttributeControlBits[0]) != E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif

    
    #if (defined CLD_ZLL_COMMISSION) && (defined ZLL_COMMISSION_SERVER)
        /* Create an instance of a TL commissioning cluster as a server */
        if(eCLD_ZllCommissionCreateCommission(&psDeviceInfo->sClusterInstance.sZllCommissionServer,
                              TRUE,
                              &sCLD_ZllCommission,
                              &psDeviceInfo->sZllCommissionServerCluster,
                              &au8ZllCommissionAttributeControlBits[0],
                              &psDeviceInfo->sZllCommissionServerCustomDataStructure) != E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif
    
    /* Recommended Optional client clusters */
    #if (defined CLD_OTA) && (defined OTA_CLIENT)
        /* Create an instance of an OTA cluster as a client */
       if(eOTA_Create(&psDeviceInfo->sClusterInstance.sOTAClient,
                      FALSE,  /* client */
                      &sCLD_OTA,
                      &psDeviceInfo->sCLD_OTA,  /* cluster definition */
                      u8EndPointIdentifier,
                      NULL,
                      &psDeviceInfo->sCLD_OTA_CustomDataStruct)!= E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif
    
    #if (defined CLD_ILLUMINANCE_MEASUREMENT) && (defined ILLUMINANCE_MEASUREMENT_CLIENT)
        /* Create an instance of an Illuminance Measurement cluster as a client */
        if(eCLD_IlluminanceMeasurementCreateIlluminanceMeasurement(
                              &psDeviceInfo->sClusterInstance.sIlluminanceMeasurementClient,
                              FALSE,
                              &sCLD_IlluminanceMeasurement,
                              &psDeviceInfo->sIlluminanceMeasurementClientCluster,
                              &au8IlluminanceMeasurementAttributeControlBits[0])!= E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif

    #if (defined CLD_ILLUMINANCE_LEVEL_SENSING) && (defined ILLUMINANCE_LEVEL_SENSING_CLIENT)
        /* Create an instance of an Illuminance Level Sensing  cluster as a client */
        if(eCLD_IlluminanceLevelSensingCreateIlluminanceLevelSensing(
                              &psDeviceInfo->sClusterInstance.sIlluminanceLevelSensingClient,
                              FALSE,
                              &sCLD_IlluminanceLevelSensing,
                              &psDeviceInfo->sIlluminanceLevelSensingClientCluster,
                              &au8IlluminanceLevelSensingAttributeControlBits[0])!= E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
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
                              &au8OccupancySensingAttributeControlBits[0])!= E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
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

