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
 * MODULE:             Lighting and occupancy
 *
 * COMPONENT:
 *
 * DESCRIPTION:        Zigbee Control bridge functions.
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zps_apl.h"
#include "zcl_heap.h"
#include "control_bridge.h"
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
 * NAME: eZLO_RegisterControlBridgeEndPoint
 *
 * DESCRIPTION:
 * Registers a control bridge device with the ZCL layer
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

PUBLIC teZCL_Status eZLO_RegisterControlBridgeEndPoint ( uint8                         u8EndPointIdentifier,
                                                         tfpZCL_ZCLCallBackFunction    cbCallBack,
                                                         tsZLO_ControlBridgeDevice     *psDeviceInfo )
{
    /* Fill in end point details */
    psDeviceInfo->sEndPoint.u8EndPointNumber               =  u8EndPointIdentifier;
    psDeviceInfo->sEndPoint.u16ManufacturerCode            =  ZCL_MANUFACTURER_CODE;
    psDeviceInfo->sEndPoint.u16ProfileEnum                 =  HA_PROFILE_ID;
    psDeviceInfo->sEndPoint.bIsManufacturerSpecificProfile =  FALSE;
    psDeviceInfo->sEndPoint.u16NumberOfClusters            =  sizeof( tsZLO_ControlBridgeDeviceClusterInstances ) / sizeof ( tsZCL_ClusterInstance );
    psDeviceInfo->sEndPoint.psClusterInstance              =  (tsZCL_ClusterInstance*)&psDeviceInfo->sClusterInstance;
    psDeviceInfo->sEndPoint.bDisableDefaultResponse        =  ZCL_DISABLE_DEFAULT_RESPONSES;
    psDeviceInfo->sEndPoint.pCallBackFunctions             =  cbCallBack;


    #if (defined CLD_BASIC) && (defined BASIC_SERVER)
        /* Create an instance of a Basic cluster as a server */
        if ( eCLD_BasicCreateBasic( &psDeviceInfo->sClusterInstance.sBasicServer,
                               TRUE,
                               &sCLD_Basic,
                               &psDeviceInfo->sBasicServerCluster,
                               &au8BasicClusterAttributeControlBits [ 0 ] ) != E_ZCL_SUCCESS )
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
    
    /* Recommended Optional Server Cluster */
    #if (defined CLD_ZLL_COMMISSION) && (defined ZLL_COMMISSION_SERVER)
        /* Create an instance of a basic cluster as a server */
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
    
    #if (defined CLD_OTA) && (defined OTA_SERVER)
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
    
    /*
     * Mandatory client clusters
     */
     
    #if (defined CLD_BASIC) && (defined BASIC_CLIENT)
        /* Create an instance of a Basic cluster as a client */
        if(eCLD_BasicCreateBasic(&psDeviceInfo->sClusterInstance.sBasicClient,
                              FALSE,
                              &sCLD_Basic,
                              &psDeviceInfo->sBasicClientCluster,
                              &au8BasicClusterAttributeControlBits[0]) != E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }    
    #endif
    
    #if (defined CLD_IDENTIFY) && (defined IDENTIFY_CLIENT)
        /* Create an instance of an Identify cluster as a client */
        if ( eCLD_IdentifyCreateIdentify ( &psDeviceInfo->sClusterInstance.sIdentifyClient,
                                      FALSE,
                                      &sCLD_Identify,
                                      &psDeviceInfo->sIdentifyClientCluster,
                                      &au8IdentifyAttributeControlBits[0],
                                      &psDeviceInfo->sIdentifyClientCustomDataStructure ) != E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif
    
    #if (defined CLD_GROUPS) && (defined GROUPS_CLIENT)
        /* Create an instance of a Groups cluster as a client */
        if ( eCLD_GroupsCreateGroups ( &psDeviceInfo->sClusterInstance.sGroupsClient,
                                  FALSE,
                                  &sCLD_Groups,
                                  &psDeviceInfo->sGroupsClientCluster,
                                  &au8GroupsAttributeControlBits[0],
                                  &psDeviceInfo->sGroupsClientCustomDataStructure,
                                  &psDeviceInfo->sEndPoint ) != E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif
    
    #if (defined CLD_SCENES) && (defined SCENES_CLIENT)
        /* Create an instance of a Scenes cluster as a client */
        if ( eCLD_ScenesCreateScenes ( &psDeviceInfo->sClusterInstance.sScenesClient,
                                  FALSE,
                                  &sCLD_Scenes,
                                  &psDeviceInfo->sScenesClientCluster,
                                  &au8ScenesAttributeControlBits[0],
                                  &psDeviceInfo->sScenesClientCustomDataStructure,
                                  &psDeviceInfo->sEndPoint ) != E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif
    
    #if (defined CLD_ONOFF) && (defined ONOFF_CLIENT)
        /* Create an instance of an On/Off cluster as a client */
        if ( eCLD_OnOffCreateOnOff ( &psDeviceInfo->sClusterInstance.sOnOffClient,
                                FALSE,
                                &sCLD_OnOffClient,
                                &psDeviceInfo->sOnOffClientCluster,
                                &au8OnOffClientAttributeControlBits[0],
                                NULL  /* no cust data struct for client */ )!= E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif


    #if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_CLIENT)
        /* Create an instance of a Level Control cluster as a client */
        if ( eCLD_LevelControlCreateLevelControl ( &psDeviceInfo->sClusterInstance.sLevelControlClient,
                                              FALSE,
                                              &sCLD_LevelControlClient,
                                              &psDeviceInfo->sLevelControlClientCluster,
                                              &au8LevelControlClientAttributeControlBits[0],
                                              &psDeviceInfo->sLevelControlClientCustomDataStructure ) != E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif

    #if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_CLIENT)
        /* Create an instance of a Colour Control cluster as a client */
        if(eCLD_ColourControlCreateColourControl(
                              &psDeviceInfo->sClusterInstance.sColourControlClient,
                              FALSE,
                              &sCLD_ColourControl,
                              &psDeviceInfo->sColourControlClientCluster,
                              &au8ColourControlAttributeControlBits[0],
                              &psDeviceInfo->sColourControlClientCustomDataStructure) != E_ZCL_SUCCESS)
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif

    
    /* Recommended optional client */
    
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

    #if (defined CLD_ZLL_COMMISSION) && (defined ZLL_COMMISSION_CLIENT)
        /* Create an instance of a zll commissioning cluster as a client */
        if(eCLD_ZllCommissionCreateCommission(&psDeviceInfo->sClusterInstance.sZllCommissionClient,
                              FALSE,
                              &sCLD_ZllCommission,
                              &psDeviceInfo->sZllCommissionClientCluster,
                              &au8ZllCommissionAttributeControlBits[0],
                              &psDeviceInfo->sZllCommissionClientCustomDataStructure) != E_ZCL_SUCCESS)
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif
    
    /* Optional server */
    #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
        /* Create an instance of an On/Off cluster as a server */
        if ( eCLD_OnOffCreateOnOff ( &psDeviceInfo->sClusterInstance.sOnOffServer,
                                TRUE,
                                &sCLD_OnOff,
                                &psDeviceInfo->sOnOffServerCluster,
                                &au8OnOffAttributeControlBits[0],
                                &psDeviceInfo->sOnOffServerCustomDataStructure )!= E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif

    #if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
        /* Create an instance of a Groups cluster as a server */
        if ( eCLD_GroupsCreateGroups ( &psDeviceInfo->sClusterInstance.sGroupsServer,
                                  TRUE,
                                  &sCLD_Groups,
                                  &psDeviceInfo->sGroupsServerCluster,
                                  &au8GroupsAttributeControlBits[0],
                                  &psDeviceInfo->sGroupsServerCustomDataStructure,
                                  &psDeviceInfo->sEndPoint ) != E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif

    #if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_SERVER)
        /* Create an instance of a appliance statistics cluster as a server */
        if ( eCLD_ApplianceStatisticsCreateApplianceStatistics ( &psDeviceInfo->sClusterInstance.sASCServer,
                                                            TRUE,
                                                            &sCLD_ApplianceStatistics,
                                                            &psDeviceInfo->sASCServerCluster,
                                                            &au8ApplianceStatisticsAttributeControlBits[0],
                                                            &psDeviceInfo->sASCServerCustomDataStructure ) != E_ZCL_SUCCESS )
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif

    /* Optional CLient */
    #if (defined CLD_IASZONE) && (defined IASZONE_CLIENT)
        /* Create an instance of a IAS Zone cluster as a client */
        if( eCLD_IASZoneCreateIASZone ( &psDeviceInfo->sClusterInstance.sIASZoneClient,
                                    FALSE,
                                    &sCLD_IASZone,
                                    &psDeviceInfo->sIASZoneClientCluster,
                                    &au8IASZoneAttributeControlBits[0],
                                    &psDeviceInfo->sIASZoneClientCustomDataStructure ) != E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif
    
    #if (defined CLD_DOOR_LOCK) && (defined DOOR_LOCK_CLIENT)
        /* Create an instance of a Door Lock cluster as a client */
        if( eCLD_DoorLockCreateDoorLock ( &psDeviceInfo->sClusterInstance.sDoorLockClient,
                                      FALSE,
                                      &sCLD_DoorLock,
                                      &psDeviceInfo->sDoorLockClientCluster,
                                      &au8DoorLockAttributeControlBits[0] ) != E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif

    #if (defined CLD_SIMPLE_METERING) && (defined SM_CLIENT)
        /* Create an instance of a Simple Metering cluster as a client */
        if( eSE_SMCreate( u8EndPointIdentifier,                                    // uint8 u8Endpoint
                      FALSE,                                                       // bool_t bIsServer
                      NULL,                                                        // uint8 *pu8AttributeControlBits
                      &psDeviceInfo->sClusterInstance.sMeteringClient,             // tsZCL_ClusterInstance *psClusterInstance
                      &sCLD_SimpleMetering,                                        // tsZCL_ClusterDefinition *psClusterDefinition
                      &psDeviceInfo->sMeteringClientCustomDataStructure,           // tsSM_CustomStruct *psCustomDataStruct
                      NULL ) != E_ZCL_SUCCESS )                                     // void *pvEndPointSharedStructPtr
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }

        /* By default the SM cluster uses APS layer security, we don't need this for HA applications */
        psDeviceInfo->sClusterInstance.sMeteringClient.psClusterDefinition->u8ClusterControlFlags = E_ZCL_SECURITY_NETWORK;
    #endif



    #if (defined CLD_TEMPERATURE_MEASUREMENT) && (defined TEMPERATURE_MEASUREMENT_CLIENT)
        /* Create an instance of a Temperature Measurement cluster as a client */
        if ( eCLD_TemperatureMeasurementCreateTemperatureMeasurement ( &psDeviceInfo->sClusterInstance.sTemperatureMeasurementClient,
                                                                  FALSE,
                                                                  &sCLD_TemperatureMeasurement,
                                                                  &psDeviceInfo->sTemperatureMeasurementClientCluster,
                                                                  &au8TemperatureMeasurementAttributeControlBits[0]) != E_ZCL_SUCCESS )
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif

    #if (defined CLD_RELATIVE_HUMIDITY_MEASUREMENT) && (defined RELATIVE_HUMIDITY_MEASUREMENT_CLIENT)
        /* Create an instance of a Relative Humidity Measurement cluster as a client */
        if ( eCLD_RelativeHumidityMeasurementCreateRelativeHumidityMeasurement ( &psDeviceInfo->sClusterInstance.sRelativeHumidityMeasurementClient,
                                                                           FALSE,
                                                                           &sCLD_RelativeHumidityMeasurement,
                                                                           &psDeviceInfo->sRelativeHumidityMeasurementClientCluster,
                                                                           &au8RelativeHumidityMeasurementAttributeControlBits[0]) != E_ZCL_SUCCESS )
        {
           // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
           return E_ZCL_FAIL;
        }
    #endif

    #if (defined CLD_THERMOSTAT) && (defined THERMOSTAT_CLIENT)
        /* Create an instance of a Thermostat cluster as a client */
        if ( eCLD_ThermostatCreateThermostat ( &psDeviceInfo->sClusterInstance.sThermostatClient,
                                          FALSE,
                                          &sCLD_Thermostat,
                                          &psDeviceInfo->sThermostatClientCluster,
                                          &au8ThermostatAttributeControlBits[0],
                                          &psDeviceInfo->sThermostatClientCustomDataStructure ) != E_ZCL_SUCCESS )
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif
    
    #if (defined CLD_APPLIANCE_STATISTICS) && (defined APPLIANCE_STATISTICS_CLIENT)
        /* Create an instance of a appliance statistics cluster as a server */
        if ( eCLD_ApplianceStatisticsCreateApplianceStatistics ( &psDeviceInfo->sClusterInstance.sASCClient,
                                                            FALSE,
                                                            &sCLD_ApplianceStatistics,
                                                            &psDeviceInfo->sASCClientCluster,
                                                            &au8ApplianceStatisticsAttributeControlBits[0],
                                                            &psDeviceInfo->sASCClientCustomDataStructure ) != E_ZCL_SUCCESS )
        {
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif

    #if (defined CLD_DIAGNOSTICS) && (defined DIAGNOSTICS_CLIENT)
        /* Create an instance of a Diagnostic cluster as a client */
        if ( eCLD_DiagnosticsCreateDiagnostics ( &psDeviceInfo->sClusterInstance.sDiagnosticClient,
                                              FALSE,
                                              &sCLD_Diagnostics,
                                              &psDeviceInfo->sDiagnosticClientCluster,
                                              &au8DiagnosticsClusterAttributeControlBits[0] ) != E_ZCL_SUCCESS )
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

