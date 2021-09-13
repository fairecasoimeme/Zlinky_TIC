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
 * MODULE:             Device Temperature Configuration Cluster
 *
 * COMPONENT:          DeviceTemperatureConfiguration.c
 *
 * DESCRIPTION:        Device Temperature Configuration cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "DeviceTemperatureConfiguration.h"
#include "zcl_options.h"


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
const tsZCL_AttributeDefinition asCLD_DeviceTemperatureConfigurationClusterAttributeDefinitions[] = {
#ifdef DEVICE_TEMPERATURE_CONFIGURATION_SERVER
            /* Device Temperature Information attribute set attribute ID's (3.4.2.2.1) */
            {E_CLD_DEVTEMPCFG_ATTR_ID_CURRENT_TEMPERATURE,          E_ZCL_AF_RD,                E_ZCL_INT16,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->i16CurrentTemperature),0},

    #ifdef CLD_DEVTEMPCFG_ATTR_ID_MIN_TEMP_EXPERIENCED
            {E_CLD_DEVTEMPCFG_ATTR_ID_MIN_TEMP_EXPERIENCED,         E_ZCL_AF_RD,                E_ZCL_INT16,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->i16MinTempExperienced),0},
    #endif

    #ifdef CLD_DEVTEMPCFG_ATTR_ID_MAX_TEMP_EXPERIENCED
            {E_CLD_DEVTEMPCFG_ATTR_ID_MAX_TEMP_EXPERIENCED,         E_ZCL_AF_RD,                E_ZCL_INT16,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->i16MaxTempExperienced),0},
    #endif

    #ifdef CLD_DEVTEMPCFG_ATTR_ID_OVER_TEMP_TOTAL_DWELL
            {E_CLD_DEVTEMPCFG_ATTR_ID_OVER_TEMP_TOTAL_DWELL,        E_ZCL_AF_RD,                E_ZCL_UINT16,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->u16OverTempTotalDwell),0},
    #endif

            /* Device Temperature settings attribute set attribute ID's (3.4.2.2.2) */
    #ifdef CLD_DEVTEMPCFG_ATTR_ID_DEVICE_TEMP_ALARM_MASK
            {E_CLD_DEVTEMPCFG_ATTR_ID_DEVICE_TEMP_ALARM_MASK,       (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_BMAP8,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->u8DeviceTempAlarmMask),0},
    #endif

    #ifdef CLD_DEVTEMPCFG_ATTR_ID_LOW_TEMP_THRESHOLD
            {E_CLD_DEVTEMPCFG_ATTR_ID_LOW_TEMP_THRESHOLD,           (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_INT16,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->i16LowTempThreshold),0},
    #endif

    #ifdef CLD_DEVTEMPCFG_ATTR_ID_HIGH_TEMP_THRESHOLD
            {E_CLD_DEVTEMPCFG_ATTR_ID_HIGH_TEMP_THRESHOLD,          (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_INT16,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->i16HighTempThreshold),0},
    #endif

    #ifdef CLD_DEVTEMPCFG_ATTR_ID_LOW_TEMP_DWELL_TRIP_POINT
            {E_CLD_DEVTEMPCFG_ATTR_ID_LOW_TEMP_DWELL_TRIP_POINT,    (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT24,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->u24LowTempDwellTripPoint),0},
    #endif

    #ifdef CLD_DEVTEMPCFG_ATTR_ID_HIGH_TEMP_DWELL_TRIP_POINT
            {E_CLD_DEVTEMPCFG_ATTR_ID_HIGH_TEMP_DWELL_TRIP_POINT,   (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT24,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->u24HighTempDwellTripPoint),0},
    #endif
#endif    

            {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                      (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

            {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                 (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_UINT16,   (uint32)(&((tsCLD_DeviceTemperatureConfiguration*)(0))->u16ClusterRevision),0},   /* Mandatory  */

     };

tsZCL_ClusterDefinition sCLD_DeviceTemperatureConfiguration = {
        GENERAL_CLUSTER_ID_DEVICE_TEMPERATURE_CONFIGURATION,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_DeviceTemperatureConfigurationClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_DeviceTemperatureConfigurationClusterAttributeDefinitions,
        NULL
};

uint8 au8DeviceTempConfigClusterAttributeControlBits[(sizeof(asCLD_DeviceTemperatureConfigurationClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_DeviceTemperatureConfigurationCreateDeviceTemperatureConfiguration
 **
 ** DESCRIPTION:
 ** Creates a device temperature configuration cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_DeviceTemperatureConfigurationCreateDeviceTemperatureConfiguration(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits)
{

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL)  )
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Create an instance of a device temp configuration cluster */
    vZCL_InitializeClusterInstance(
                               psClusterInstance, 
                               bIsServer,
                               psClusterDefinition,
                               pvEndPointSharedStructPtr,
                               pu8AttributeControlBits,
                               NULL,
                               NULL);

    if(psClusterInstance->pvEndPointSharedStructPtr != NULL)
    {
#ifdef DEVICE_TEMPERATURE_CONFIGURATION_SERVER 
        /* Initialise attribute defaults */
        #ifdef CLD_DEVTEMPCFG_ATTR_ID_OVER_TEMP_TOTAL_DWELL
            ((tsCLD_DeviceTemperatureConfiguration*)psClusterInstance->pvEndPointSharedStructPtr)->u16OverTempTotalDwell = 0x00;
        #endif

        #ifdef CLD_DEVTEMPCFG_ATTR_ID_DEVICE_TEMP_ALARM_MASK
            ((tsCLD_DeviceTemperatureConfiguration*)psClusterInstance->pvEndPointSharedStructPtr)->u8DeviceTempAlarmMask = 0x00;
        #endif
#endif
        ((tsCLD_DeviceTemperatureConfiguration*)psClusterInstance->pvEndPointSharedStructPtr)->u32FeatureMap = CLD_DEVTEMPCFG_FEATURE_MAP;
        ((tsCLD_DeviceTemperatureConfiguration*)psClusterInstance->pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_DEVTEMPCFG_CLUSTER_REVISION;
    }

    return E_ZCL_SUCCESS;

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

