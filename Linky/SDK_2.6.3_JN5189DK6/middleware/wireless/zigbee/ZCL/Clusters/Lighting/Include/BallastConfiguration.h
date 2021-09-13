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
 * COMPONENT:          BallastConfiguration.h
 *
 * DESCRIPTION:        Header for Ballast Configuration Cluster
 *
 *****************************************************************************/

#ifndef BALLAST_CONFIGURATION_H
#define BALLAST_CONFIGURATION_H

#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define LIGHTING_CLUSTER_ID_BALLAST_CONFIGURATION   0x0301

/****************************************************************************/
/*       Ballast Configuration Cluster - Optional Attributes                */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the simple metering cluster.                               */
/****************************************************************************/

    /* Ballast Information attribute set attribute ID's (5.3.2.2.1) */
#define CLD_BALLASTCONFIGURATION_ATTR_BALLAST_STATUS
    /* Ballast Settings attribute attribute ID's set (5.3.2.2.2) */
#define CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_LEVEL
#define CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_FADE_TIME
#define CLD_BALLASTCONFIGURATION_ATTR_INTRINSIC_BALLAST_FACTOR
#define CLD_BALLASTCONFIGURATION_ATTR_BALLAST_FACTOR_ADJUSTMENT

    /* Lamp Information attribute attribute ID's set (5.3.2.2.3) */
#define CLD_BALLASTCONFIGURATION_ATTR_LAMP_QUANTITY

    /* Lamp Settings attribute ID's set (5.3.2.2.4) */
#define CLD_BALLASTCONFIGURATION_ATTR_LAMP_TYPE
#define CLD_BALLASTCONFIGURATION_ATTR_LAMP_MANUFACTURER
#define CLD_BALLASTCONFIGURATION_ATTR_LAMP_RATED_HOURS
#define CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS
#define CLD_BALLASTCONFIGURATION_ATTR_LAMP_ALARM_MODE
#define CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS_TRIP_POINT

/* End of optional attributes */

/* Ballast Status Bitmask */
#define E_CLD_BC_BALLAST_STATUS_NON_OPERATIONAL_BIT     0
#define E_CLD_BC_BALLAST_STATUS_LAMP_NOT_IN_SOCKET_BIT  1

/* Lamp Alarm Mode Bitmask */
#define E_CLD_BC_LAMP_ALARM_MODE_LAMP_BURN_HOURS_BIT    0

#ifndef CLD_BALLASTCONFIGURATION_CLUSTER_REVISION
    #define CLD_BALLASTCONFIGURATION_CLUSTER_REVISION        2
#endif 

#ifndef CLD_BALLASTCONFIGURATION_FEATURE_MAP
    #define CLD_BALLASTCONFIGURATION_FEATURE_MAP             0
#endif 
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Attribute ID's */
typedef enum 
{
    /* Ballast Information attribute set attribute ID's (5.3.2.2.1) */
    E_CLD_BALLASTCONFIGURATION_ATTR_PHYSICAL_MIN_LEVEL      = 0x0000,   /* Mandatory */
    E_CLD_BALLASTCONFIGURATION_ATTR_PHYSICAL_MAX_LEVEL,                 /* Mandatory */
    E_CLD_BALLASTCONFIGURATION_ATTR_BALLAST_STATUS,                     /* Optional */

    /* Ballast Settings attribute attribute ID's set (5.3.2.2.2) */
    E_CLD_BALLASTCONFIGURATION_ATTR_MIN_LEVEL               = 0x0010,   /* Mandatory */
    E_CLD_BALLASTCONFIGURATION_ATTR_MAX_LEVEL,                          /* Mandatory */
    E_CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_LEVEL,                     /* Deprecated */
    E_CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_FADE_TIME,                 /* Deprecated */
    E_CLD_BALLASTCONFIGURATION_ATTR_INTRINSIC_BALLAST_FACTOR,           /* Optional */
    E_CLD_BALLASTCONFIGURATION_ATTR_BALLAST_FACTOR_ADJUSTMENT,          /* Optional */

    /* Lamp Information attribute attribute ID's set (5.3.2.2.3) */
    E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_QUANTITY           = 0x0020,   /* Optional */

    /* Lamp Settings attribute ID's set (5.3.2.2.4) */
    E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_TYPE               = 0x0030,   /* Optional */
    E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_MANUFACTURER,                  /* Optional */
    E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_RATED_HOURS,                   /* Optional */
    E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS,                    /* Optional */
    E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_ALARM_MODE,                    /* Optional */
    E_CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS_TRIP_POINT,         /* Optional */
} teCLD_BallastConfiguration_ClusterID;


/* Ballast Configuration Cluster */
typedef struct
{
#ifdef BALLAST_CONFIGURATION_SERVER
    /* Ballast Information attribute set attribute ID's (5.3.2.2.1) */
    zuint8                  u8PhysicalMinLevel;

    zuint8                  u8PhysicalMaxLevel;

#ifdef CLD_BALLASTCONFIGURATION_ATTR_BALLAST_STATUS
    zbmap8                  u8BallastStatus;
#endif
    
    /* Ballast Settings attribute attribute ID's set (5.3.2.2.2) */

    zuint8                  u8MinLevel;

    zuint8                  u8MaxLevel;

#ifdef CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_LEVEL
    zuint8                  u8PowerOnLevel;
#endif

#ifdef CLD_BALLASTCONFIGURATION_ATTR_POWER_ON_FADE_TIME
    zuint16                 u16PowerOnFadeTime;
#endif

#ifdef CLD_BALLASTCONFIGURATION_ATTR_INTRINSIC_BALLAST_FACTOR
    zuint8                  u8IntrinsicBallastFactor;
#endif

#ifdef CLD_BALLASTCONFIGURATION_ATTR_BALLAST_FACTOR_ADJUSTMENT
    zuint8                  u8BallastFactorAdjustment;
#endif

    /* Lamp Information attribute attribute ID's set (5.3.2.2.3) */
#ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_QUANTITY
    zuint8                  u8LampQuantity;
#endif

    /* Lamp Settings attribute ID's set (5.3.2.2.4) */
#ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_TYPE
    tsZCL_CharacterString   sLampType;
    uint8                   au8LampType[16];
#endif

#ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_MANUFACTURER
    tsZCL_CharacterString   sLampManufacturer;
    uint8                   au8LampManufacturer[16];
#endif

#ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_RATED_HOURS
    zuint24                 u32LampRatedHours;
#endif

#ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS
    zuint24                 u32LampBurnHours;
#endif

#ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_ALARM_MODE
    zbmap8                  u8LampAlarmMode;
#endif

#ifdef CLD_BALLASTCONFIGURATION_ATTR_LAMP_BURN_HOURS_TRIP_POINT
    zuint24                 u32LampBurnHoursTripPoint;
#endif
#endif
    zbmap32                 u32FeatureMap;

    zuint16                 u16ClusterRevision;

} tsCLD_BallastConfiguration;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_BallastConfigurationCreateBallastConfiguration(
                    tsZCL_ClusterInstance              *psClusterInstance,
                    bool_t                              bIsServer,
                    tsZCL_ClusterDefinition            *psClusterDefinition,
                    void                               *pvEndPointSharedStructPtr,
                    uint8                              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZCL_ClusterDefinition sCLD_BallastConfiguration;
extern uint8 au8BallastConfigurationAttributeControlBits[];
extern const tsZCL_AttributeDefinition asCLD_BallastConfigurationClusterAttributeDefinitions[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* BALLAST_CONFIGURATION_H */
