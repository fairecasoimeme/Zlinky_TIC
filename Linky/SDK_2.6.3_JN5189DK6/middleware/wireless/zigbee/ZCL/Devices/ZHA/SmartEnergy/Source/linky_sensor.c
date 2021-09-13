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
 * MODULE:             Light Sensor
 *
 * COMPONENT:          light_sensor.c
 *
 * DESCRIPTION:        ZigBee Light Sensor profile functions
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/



#include <jendefs.h>
#include <string.h>
#include "zps_apl.h"
#include "zcl_heap.h"
#include "linky_sensor.h"

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
 * NAME: eZLO_RegisterSimpleMeteringEndPoint
 *
 * DESCRIPTION:
 * Registers a Metering sensor device with the ZCL layer
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
PUBLIC teZCL_Status eZLO_RegisterSimpleMeteringEndPoint(uint8 u8EndPointIdentifier,
                                              tfpZCL_ZCLCallBackFunction cbCallBack,
                                              tsZLO_LinkyDevice *psDeviceInfo)
{

    /* Fill in end point details */
    psDeviceInfo->sEndPoint.u8EndPointNumber = u8EndPointIdentifier;
    psDeviceInfo->sEndPoint.u16ManufacturerCode = ZCL_MANUFACTURER_CODE;
    psDeviceInfo->sEndPoint.u16ProfileEnum = HA_PROFILE_ID;
    psDeviceInfo->sEndPoint.bIsManufacturerSpecificProfile = FALSE;
    psDeviceInfo->sEndPoint.u16NumberOfClusters = sizeof(tsZLO_LinkyDeviceClusterInstances) / sizeof(tsZCL_ClusterInstance);
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
								au8ElectricalMeasurementAttributeControlBits)!= E_ZCL_SUCCESS)
		{
			return E_ZCL_FAIL;
		}
	#endif

    /* Optional server clusters */
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
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        }
    #endif


    /* Recommended Optional client clusters */
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
            // Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
            return E_ZCL_FAIL;
        } 
    #endif

    #if(defined CLD_OTA) && (defined OTA_CLIENT)
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
    
	if(eCLD_LixeeCreateLinky(&psDeviceInfo->sClusterInstance.sLinkyServer,
						  TRUE,
						  &sCLD_Linky,
						  &psDeviceInfo->sLinkyServerCluster,
						  NULL) != E_ZCL_SUCCESS)
	{
		// Need to convert from cluster specific to ZCL return type so we lose the extra information of the return code
		return E_ZCL_FAIL;
	}

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

    return eZCL_Register(&psDeviceInfo->sEndPoint);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

