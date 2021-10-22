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
 * MODULE:             Ballast Configuration Cluster
 *
 * COMPONENT:          BallastConfiguration.c
 *
 * DESCRIPTION:        Ballast Configuration cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"
#include "string.h"
#include "BallastConfiguration.h"


#include "dbg.h"

#ifdef DEBUG_CLD_BALLAST_CONFIGURATION
#define TRACE_BALLAST_CONFIGURATION TRUE
#else
#define TRACE_BALLAST_CONFIGURATION FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_BALLAST_CONFIGURATION) && !defined(BALLAST_CONFIGURATION_SERVER) && !defined(BALLAST_CONFIGURATION_CLIENT)
#error You Must Have either BALLAST_CONFIGURATION_SERVER and/or BALLAST_CONFIGURATION_CLIENT defined in zcl_options.h
#endif

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
const tsZCL_AttributeDefinition asCLD_BallastConfigurationClusterAttributeDefinitions[] = {
#ifdef BALLAST_CONFIGURATION_SERVER    
    /* Ballast Information attribute set attribute ID's (5.3.2.2.1) */
        {E_CLD_BALLASTCONFIGURATION_ATTR_PHYSICAL_MIN_LEVEL,        E_ZCL_AF_RD,                E_ZCL_UINT8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8PhysicalMinLevel), 0},

        {E_CLD_BALLASTCONFIGURATION_ATTR_PHYSICAL_MAX_LEVEL,        E_ZCL_AF_RD,                E_ZCL_UINT8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8PhysicalMaxLevel), 0},

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_BALLAST_STATUS
        {E_CLD_BALLASTCONFIGURATION_ATTR_BALLAST_STATUS,            E_ZCL_AF_RD,                E_ZCL_BMAP8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8BallastStatus), 0},
    #endif
        /* Ballast Settings attribute attribute ID's set (5.3.2.2.2) */
        {E_CLD_BALLASTCONFIGURATION_ATTR_MIN_LEVEL,                 (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8MinLevel), 0},

        {E_CLD_BALLASTCONFIGURATION_ATTR_MAX_LEVEL,                 (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8MaxLevel), 0},

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_LEVEL
        {E_CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_LEVEL,            (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8PowerOnLevel), 0},
    #endif

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_FADE_TIME
        {E_CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_FADE_TIME,        (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT16,  (uint32)(&((tsCLD_BallastConfiguration*)(0))->u16PowerOnFadeTime), 0},
    #endif

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_INTRINSIC_BALLAST_FACTOR
        {E_CLD_BALLASTCONFIGURATION_ATTR_INTRINSIC_BALLAST_FACTOR,  (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8IntrinsicBallastFactor), 0},
    #endif

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_BALLAST_FACTOR_ADJUSTMENT
        {E_CLD_BALLASTCONFIGURATION_ATTR_BALLAST_FACTOR_ADJUSTMENT, (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8BallastFactorAdjustment), 0},
    #endif

        /* Lamp Information attribute attribute ID's set (5.3.2.2.3) */
    #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_QUANTITY
        {E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_QUANTITY,             E_ZCL_AF_RD,                E_ZCL_UINT8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8LampQuantity), 0},
    #endif

        /* Lamp Settings attribute ID's set (5.3.2.2.4) */
    #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_TYPE
        {E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_TYPE,                 (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_CSTRING, (uint32)(&((tsCLD_BallastConfiguration*)(0))->sLampType), 0},
    #endif

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_MANUFACTURER
        {E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_MANUFACTURER,         (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_CSTRING, (uint32)(&((tsCLD_BallastConfiguration*)(0))->sLampManufacturer), 0},
    #endif

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_RATED_HOURS
        {E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_RATED_HOURS,          (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT24,  (uint32)(&((tsCLD_BallastConfiguration*)(0))->u32LampRatedHours), 0},
    #endif

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS
        {E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS,           (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT24,  (uint32)(&((tsCLD_BallastConfiguration*)(0))->u32LampBurnHours), 0},
    #endif

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_ALARM_MODE
        {E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_ALARM_MODE,           (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_BMAP8,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u8LampAlarmMode), 0},
    #endif

    #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS_TRIP_POINT
        {E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS_TRIP_POINT,(E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_UINT24,  (uint32)(&((tsCLD_BallastConfiguration*)(0))->u32LampBurnHoursTripPoint), 0},
    #endif
#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                          (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_BallastConfiguration*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
  
        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                     (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_UINT16,  (uint32)(&((tsCLD_BallastConfiguration*)(0))->u16ClusterRevision),0},   /* Mandatory  */

};

tsZCL_ClusterDefinition sCLD_BallastConfiguration = {
        LIGHTING_CLUSTER_ID_BALLAST_CONFIGURATION,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_BallastConfigurationClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_BallastConfigurationClusterAttributeDefinitions,
        NULL
};

uint8 au8BallastConfigurationAttributeControlBits[(sizeof(asCLD_BallastConfigurationClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_BallastConfigurationCreateBallastConfiguration
 **
 ** DESCRIPTION:
 ** Creates a Ballast Configuration cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_BallastConfigurationCreateBallastConfiguration(
        tsZCL_ClusterInstance                   *psClusterInstance,
        bool_t                                  bIsServer,
        tsZCL_ClusterDefinition                 *psClusterDefinition,
        void                                    *pvEndPointSharedStructPtr,
        uint8                                   *pu8AttributeControlBits)
{

#ifdef STRICT_PARAM_CHECK
	/* Parameter check */
	if((psClusterInstance==NULL) ||
	   (psClusterDefinition==NULL))
	{
		return E_ZCL_ERR_PARAMETER_NULL;
	}
#endif

    /* Create an instance of a ballast configuration cluster */
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   NULL,
                                   NULL);  
    /* Set default values */
        if(pvEndPointSharedStructPtr != NULL)
        {
    #ifdef BALLAST_CONFIGURATION_SERVER            
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8PhysicalMinLevel = 0x01;

                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8PhysicalMaxLevel = 0xfe;

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_BALLAST_STATUS
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8BallastStatus = 0x00;
            #endif
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8MinLevel = ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8PhysicalMinLevel;

                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8MaxLevel = ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8PhysicalMaxLevel;

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_LEVEL
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8PowerOnLevel = ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8PhysicalMaxLevel;
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_FADE_TIME
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u16PowerOnFadeTime = 0x0000;
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_INTRINSIC_BALLAST_FACTOR
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_BALLAST_FACTOR_ADJUSTMENT
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8BallastFactorAdjustment = 0xff;
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_QUANTITY
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_TYPE
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->sLampType.u8MaxLength = sizeof(((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->au8LampType);
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->sLampType.u8Length = 0;
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->sLampType.pu8Data = ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->au8LampType;
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_MANUFACTURER
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->sLampManufacturer.u8MaxLength = sizeof(((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->au8LampManufacturer);
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->sLampManufacturer.u8Length = 0;
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->sLampManufacturer.pu8Data = ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->au8LampManufacturer;
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_RATED_HOURS
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u32LampRatedHours = 0x00ffffff;
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u32LampBurnHours = 0x00;
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_ALARM_MODE
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u8LampAlarmMode = 0x00;
            #endif

            #ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS_TRIP_POINT
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u32LampBurnHoursTripPoint = 0x00ffffff;
            #endif
    #endif    
                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_BALLASTCONFIGURATION_FEATURE_MAP;

                ((tsCLD_BallastConfiguration*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_BALLASTCONFIGURATION_CLUSTER_REVISION;
        }
    return E_ZCL_SUCCESS;
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

