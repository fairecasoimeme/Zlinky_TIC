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
 * MODULE:             Occupancy Sensing Cluster
 *
 * COMPONENT:          OccupancySensing.h
 *
 * DESCRIPTION:        Header for Occupancy Sensing Cluster
 *
 *****************************************************************************/

#ifndef OCCUPANCY_SENSING_H
#define OCCUPANCY_SENSING_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef CLD_OS_CLUSTER_REVISION
    #define CLD_OS_CLUSTER_REVISION        2
#endif

#ifndef CLD_OS_FEATURE_MAP
    #define CLD_OS_FEATURE_MAP             0
#endif
/****************************************************************************/
/*       Occupancy Sensing Cluster - Optional Attributes                    */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the Occupancy Sensing cluster.                             */
/****************************************************************************/

/* Enable the optional PIR Occupied to Unoccupied Delay attribute */
//#define CLD_OS_ATTR_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY

/* Enable the optional PIR Unoccupied to Occupied Delay attribute */
//#define CLD_OS_ATTR_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY

/* Enable the optional PIR Unoccupied to Occupied threshold attribute */
//#define CLD_OS_ATTR_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD

/* Enable the optional Ultrasonic Occupied to Unoccupied Delay attribute */
//#define CLD_OS_ATTR_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY

/* Enable the optional Ultrasonic Unoccupied to Occupied Delay attribute */
//#define CLD_OS_ATTR_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY

/* Enable the optional Ultrasonic Unoccupied to Occupied threshold attribute */
//#define CLD_OS_ATTR_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD

/* Enable the optional Attribute Reporting Status attribute */
//#define CLD_OS_ATTR_ATTRIBUTE_REPORTING_STATUS

/* End of optional attributes */


/* Cluster ID's */
#define MEASUREMENT_AND_SENSING_CLUSTER_ID_OCCUPANCY_SENSING  0x0406


/* Occupancy */
#define E_CLD_OS_OCCUPIED                       (1 << 0)

/*OccupancySensorTypeBitmap*/

#define OS_OCCUPANCY_SENSOR_TYPE_BITMAP_PIR                              0
#define OS_OCCUPANCY_SENSOR_TYPE_BITMAP_ULTRASONIC                       1
#define OS_OCCUPANCY_SENSOR_TYPE_BITMAP_PHYSICAL_CONTACT                 2

#define OS_OCCUPANCY_SENSOR_TYPE_BITMAP_PIR_MASK                             (1 << OS_OCCUPANCY_SENSOR_TYPE_BITMAP_PIR)
#define OS_OCCUPANCY_SENSOR_TYPE_BITMAP_ULTRASONIC_MASK                      (1 << OS_OCCUPANCY_SENSOR_TYPE_BITMAP_ULTRASONIC)
#define OS_OCCUPANCY_SENSOR_TYPE_BITMAP_PHYSICAL_CONTACT_MASK                (1 << OS_OCCUPANCY_SENSOR_TYPE_BITMAP_PHYSICAL_CONTACT)


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Occupancy Sensor Information attribute set attribute ID's (4.8.2.2.1) */
    E_CLD_OS_ATTR_ID_OCCUPANCY                                  = 0x0000,   /* Mandatory */
    E_CLD_OS_ATTR_ID_OCCUPANCY_SENSOR_TYPE,                                 /* Mandatory */
    E_CLD_OS_ATTR_ID_OCCUPANCY_SENSOR_TYPE_BITMAP,

    /* PIR configuration set attribute ID's (4.8.2.2.2) */
    E_CLD_OS_ATTR_ID_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY           = 0x0010,
    E_CLD_OS_ATTR_ID_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY,
    E_CLD_OS_ATTR_ID_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD,
    /* Ultrasonic configuration set attribute ID's (4.8.2.2.3) */
    E_CLD_OS_ATTR_ID_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY    = 0x0020,
    E_CLD_OS_ATTR_ID_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY,
    E_CLD_OS_ATTR_ID_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD,
    /* Physical Contact configuration set attribute ID's (4.8.2.2.4) */
    E_CLD_OS_ATTR_ID_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY    = 0x0030,
    E_CLD_OS_ATTR_ID_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY,
    E_CLD_OS_ATTR_ID_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
    
} teCLD_OS_ClusterID;


/* Occupancy Sensor Types */
typedef enum 
{
    E_CLD_OS_SENSORT_TYPE_PIR,
    E_CLD_OS_SENSORT_TYPE_ULTRASONIC,
    E_CLD_OS_SENSORT_TYPE_PIR_AND_ULTRASONIC,
} teCLS_OS_SensorTypes;


/* Occupancy Sensing Cluster */
typedef struct
{
#ifdef OCCUPANCY_SENSING_SERVER    
    zbmap8                  u8Occupancy;

    zenum8                  eOccupancySensorType;

    zbmap8                  u8OccupancySensorTypeBitmap;

#ifdef CLD_OS_ATTR_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY
    zuint16                  u16PIROccupiedToUnoccupiedDelay;
#endif

#ifdef CLD_OS_ATTR_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY
    zuint16                  u16PIRUnoccupiedToOccupiedDelay;
#endif

#ifdef CLD_OS_ATTR_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
    zuint8                  u8PIRUnoccupiedToOccupiedThreshold;
#endif

#ifdef CLD_OS_ATTR_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY
    zuint16                  u16UltrasonicOccupiedToUnoccupiedDelay;
#endif

#ifdef CLD_OS_ATTR_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY
    zuint16                  u16UltrasonicUnoccupiedToOccupiedDelay;
#endif

#ifdef CLD_OS_ATTR_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
    zuint8                  u8UltrasonicUnoccupiedToOccupiedThreshold;
#endif

#ifdef CLD_OS_ATTR_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY
    zuint16                  u16PhysicalContactOccupiedToUnoccupiedDelay;
#endif

#ifdef CLD_OS_ATTR_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY
    zuint16                   u16PhysicalContactUnoccupiedToOccupiedDelay;
#endif

#ifdef CLD_OS_ATTR_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
    zuint8                  u8PhysicalContactUnoccupiedToOccupiedThreshold;
#endif
    
#ifdef CLD_OS_ATTR_ATTRIBUTE_REPORTING_STATUS
    zenum8                  u8AttributeReportingStatus;
#endif
#endif
    zbmap32                 u32FeatureMap;

    zuint16                 u16ClusterRevision;
} tsCLD_OccupancySensing;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_OccupancySensingCreateOccupancySensing(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_OccupancySensing;
extern const tsZCL_AttributeDefinition asCLD_OccupancySensingClusterAttributeDefinitions[];
extern uint8 au8OccupancySensingAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* OCCUPANCY_SENSING_H */
