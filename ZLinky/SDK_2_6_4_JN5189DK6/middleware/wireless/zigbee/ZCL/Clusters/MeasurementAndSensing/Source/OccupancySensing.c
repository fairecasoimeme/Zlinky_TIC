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
 * COMPONENT:          OccupancySensingMeasurement.c
 *
 * DESCRIPTION:        Occupancy Sensing cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "OccupancySensing.h"
#include "zcl_customcommand.h"
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
const tsZCL_AttributeDefinition asCLD_OccupancySensingClusterAttributeDefinitions[] = {
#ifdef OCCUPANCY_SENSING_SERVER
        {E_CLD_OS_ATTR_ID_OCCUPANCY,                                    (E_ZCL_AF_RD|E_ZCL_AF_RP),  E_ZCL_BMAP8,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u8Occupancy), 0},            /* Mandatory */

        {E_CLD_OS_ATTR_ID_OCCUPANCY_SENSOR_TYPE,                        E_ZCL_AF_RD,                    E_ZCL_ENUM8,    (uint32)(&((tsCLD_OccupancySensing*)(0))->eOccupancySensorType), 0},   /* Mandatory */

        {E_CLD_OS_ATTR_ID_OCCUPANCY_SENSOR_TYPE_BITMAP,                 E_ZCL_AF_RD,                    E_ZCL_BMAP8,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u8OccupancySensorTypeBitmap), 0},  /* Mandatory */
        
    #ifdef CLD_OS_ATTR_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY
        {E_CLD_OS_ATTR_ID_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY,             (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,   (uint32)(&((tsCLD_OccupancySensing*)(0))->u16PIROccupiedToUnoccupiedDelay), 0},
    #endif

    #ifdef CLD_OS_ATTR_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY
        {E_CLD_OS_ATTR_ID_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY,             (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u16PIRUnoccupiedToOccupiedDelay), 0},
    #endif

    #ifdef CLD_OS_ATTR_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
        {E_CLD_OS_ATTR_ID_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD,         (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT8,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u8PIRUnoccupiedToOccupiedThreshold), 0},
    #endif

    #ifdef CLD_OS_ATTR_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY
        {E_CLD_OS_ATTR_ID_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY,      (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,   (uint32)(&((tsCLD_OccupancySensing*)(0))->u16UltrasonicOccupiedToUnoccupiedDelay), 0},
    #endif

    #ifdef CLD_OS_ATTR_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY
        {E_CLD_OS_ATTR_ID_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY,      (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u16UltrasonicUnoccupiedToOccupiedDelay), 0},
    #endif

    #ifdef CLD_OS_ATTR_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
        {E_CLD_OS_ATTR_ID_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD,  (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT8,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u8UltrasonicUnoccupiedToOccupiedThreshold), 0},
    #endif
        
    #ifdef CLD_OS_ATTR_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY
        {E_CLD_OS_ATTR_ID_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY,      (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,   (uint32)(&((tsCLD_OccupancySensing*)(0))->u16PhysicalContactOccupiedToUnoccupiedDelay), 0},
    #endif

    #ifdef CLD_OS_ATTR_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY
        {E_CLD_OS_ATTR_ID_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY,      (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT16,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u16PhysicalContactUnoccupiedToOccupiedDelay), 0},
    #endif

    #ifdef CLD_OS_ATTR_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
        {E_CLD_OS_ATTR_ID_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD,  (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_UINT8,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u8PhysicalContactUnoccupiedToOccupiedThreshold), 0},
    #endif

#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                             (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_BMAP32,   (uint32)(&((tsCLD_OccupancySensing*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                        (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_UINT16,   (uint32)(&((tsCLD_OccupancySensing*)(0))->u16ClusterRevision),  0},   /* Mandatory  */

    #if (defined OCCUPANCY_SENSING_SERVER) && (defined CLD_OS_ATTR_ATTRIBUTE_REPORTING_STATUS)
        {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,              (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_ENUM8,    (uint32)(&((tsCLD_OccupancySensing*)(0))->u8AttributeReportingStatus), 0},  /* Optional */
    #endif
};

tsZCL_ClusterDefinition sCLD_OccupancySensing = {
        MEASUREMENT_AND_SENSING_CLUSTER_ID_OCCUPANCY_SENSING,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_OccupancySensingClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_OccupancySensingClusterAttributeDefinitions,
        NULL
};
uint8 au8OccupancySensingAttributeControlBits[(sizeof(asCLD_OccupancySensingClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_OccupancySensingCreateOccupancySensing
 **
 ** DESCRIPTION:
 ** Creates an occupancy sensing cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_OccupancySensingCreateOccupancySensing(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits)
{

    #ifdef STRICT_PARAM_CHECK 
         /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Create an instance of a temperature measurement cluster */
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   NULL,
                                   NULL);    
 
        if(pvEndPointSharedStructPtr != NULL)
        {
        #ifdef OCCUPANCY_SENSING_SERVER         
            #ifdef CLD_OS_ATTR_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
                ((tsCLD_OccupancySensing*)pvEndPointSharedStructPtr)->u8PIRUnoccupiedToOccupiedThreshold        = 0x1;
            #endif
            #ifdef CLD_OS_ATTR_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
                ((tsCLD_OccupancySensing*)pvEndPointSharedStructPtr)->u8UltrasonicUnoccupiedToOccupiedThreshold = 0x1;
            #endif
            #ifdef CLD_OS_ATTR_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD
                ((tsCLD_OccupancySensing*)pvEndPointSharedStructPtr)->u8PhysicalContactUnoccupiedToOccupiedThreshold = 0x1;
            #endif
        #endif    
                ((tsCLD_OccupancySensing*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_OS_FEATURE_MAP;

                ((tsCLD_OccupancySensing*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_OS_CLUSTER_REVISION;
        }
        
    /* As this cluster has reportable attributes enable default reporting */
    vZCL_SetDefaultReporting(psClusterInstance);
    
    return E_ZCL_SUCCESS;

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

