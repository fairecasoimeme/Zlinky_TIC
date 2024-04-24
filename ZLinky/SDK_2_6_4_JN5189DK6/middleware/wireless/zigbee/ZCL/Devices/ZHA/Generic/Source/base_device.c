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
 * MODULE:             Base Device template
 *
 * COMPONENT:          base_device.c
 *
 * DESCRIPTION:        ZigBee Base Device Template
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

#include <string.h>
#include "zps_apl.h"
#include "zcl_heap.h"
#include "zcl.h"
#include "zcl_options.h"
#include "base_device.h"

#include "dbg.h"
/*#include "Basic.h"
#include "Identify.h"
#include "Groups.h"
#include "OnOff.h"
*/


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
 * NAME: eHA_RegisterBaseDevicePoint
 *
 * DESCRIPTION:
 * Registers an base device with the ZCL layer
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
PUBLIC teZCL_Status eZHA_RegisterBaseDeviceEndPoint(uint8 u8EndPointIdentifier,
                                              tfpZCL_ZCLCallBackFunction cbCallBack,
                                              tsZHA_BaseDevice *psDeviceInfo)
{

    /* Fill in end point details */
    psDeviceInfo->sEndPoint.u8EndPointNumber = u8EndPointIdentifier;
    psDeviceInfo->sEndPoint.u16ManufacturerCode = ZCL_MANUFACTURER_CODE;
    psDeviceInfo->sEndPoint.u16ProfileEnum = HA_PROFILE_ID;
    psDeviceInfo->sEndPoint.bIsManufacturerSpecificProfile = FALSE;
    psDeviceInfo->sEndPoint.u16NumberOfClusters = sizeof(tsZHA_BaseDeviceClusterInstances) / sizeof(tsZCL_ClusterInstance);
    psDeviceInfo->sEndPoint.psClusterInstance = (tsZCL_ClusterInstance*)&psDeviceInfo->sClusterInstance;
    psDeviceInfo->sEndPoint.bDisableDefaultResponse = ZCL_DISABLE_DEFAULT_RESPONSES;
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

/* Mandatory client clusters */
#if (defined CLD_BASIC) && (defined BASIC_CLIENT)
    /* Create an instance of a Basic cluster as a client */
    if(eCLD_BasicCreateBasic(&psDeviceInfo->sClusterInstance.sBasicClient,
                          FALSE,
                          &sCLD_Basic,
                          &psDeviceInfo->sBasicClientCluster,
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

#if (defined CLD_SIMPLE_METERING) && (defined SM_SERVER)
        /* Create an instance of a simple metering cluster as a server */
            sCLD_SimpleMetering.u8ClusterControlFlags = E_ZCL_SECURITY_NETWORK;
            if (eSE_SMCreate(u8EndPointIdentifier,
                    TRUE,
                    &au8SimpleMeteringServerAttributeControlBits[0],
                    &psDeviceInfo->sClusterInstance.sSimpleMeteringServer,
                    &sCLD_SimpleMetering,
                    &psDeviceInfo->sSimpleMeteringServerCustomDataStruct,
                    &psDeviceInfo->sSimpleMeteringServerCluster) != E_ZCL_SUCCESS)
            {
                // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
                return E_ZCL_FAIL;
            }
#endif

#if (defined CLD_ELECTRICAL_MEASUREMENT && defined ELECTRICAL_MEASUREMENT_SERVER)
	// Create an instance of an 0x0b04 cluster as a server
	if (eCLD_ElectricalMeasurementCreateElectricalMeasurement(
							&psDeviceInfo->sClusterInstance.sElectricalMeasurementServer,
							TRUE,
							&sCLD_ElectricalMeasurement,
							&psDeviceInfo->sElectricalMeasurement,
							&au8ElectricalMeasurementAttributeControlBits)!= E_ZCL_SUCCESS)
	{
		return E_ZCL_FAIL;
	}
#endif

#if (defined CLD_METER_IDENTIFICATION) && (defined METER_IDENTIFICATION_SERVER)
	if(eCLD_MeterIdentificationCreateMeterIdentification(&psDeviceInfo->sClusterInstance.sMeterIdentificationServer,
							  TRUE,
							  &sCLD_MeterIdentification,
							  &psDeviceInfo->sMeterIdentification,
							  NULL) != E_ZCL_SUCCESS)
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
        return E_ZCL_FAIL;
    } 
#endif   

#if (defined CLD_ONOFF) && (defined ONOFF_CLIENT)
    /* Create an instance of an On/Off cluster as a client */
    if(eCLD_OnOffCreateOnOff(&psDeviceInfo->sClusterInstance.sOnOffClient,
                          FALSE,
                          &sCLD_OnOffClient,
                          &psDeviceInfo->sOnOffClientCluster,
                          &au8OnOffClientAttributeControlBits[0],
                          NULL) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    } 
#endif

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

    if(eCLD_LixeeCreateLinky(&psDeviceInfo->sClusterInstance.sLinkyServer,
						  TRUE,
						  &sCLD_Linky,
						  &psDeviceInfo->sLinkyServerCluster,
						  NULL) != E_ZCL_SUCCESS)
	{
		// Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
		return E_ZCL_FAIL;
	}

	#if (defined CLD_TUYASPECIFIC)
        /* Create an instance of a Basic cluster as a server */
        if(eCLD_TuyaSpecificCreateLinky(&psDeviceInfo->sClusterInstance.sTuyaSpecificServer,
                              TRUE,
                              &sCLD_TuyaSpecific,
                              &psDeviceInfo->sTuyaSpecificServerCluster,
                              NULL) != E_ZCL_SUCCESS)
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
        return E_ZCL_FAIL;
    }
#endif

#if (defined CLD_OTA) && (defined OTA_SERVER)
    /* Create an instance of an OTA cluster as a client */
    if(eOTA_Create(&psDeviceInfo->sClusterInstance.sOTAServer,
                      TRUE,  /* client */
                      &sCLD_OTA,
                      &psDeviceInfo->sCLD_OTAServer,  /* cluster definition */
                      u8EndPointIdentifier,
                      NULL,
                      &psDeviceInfo->sCLD_OTA_CustomDataStruct)!= E_ZCL_SUCCESS)
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

