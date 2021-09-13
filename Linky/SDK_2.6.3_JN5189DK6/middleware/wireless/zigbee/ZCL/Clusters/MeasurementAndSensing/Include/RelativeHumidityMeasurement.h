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
 * MODULE:             Relative Humidity Measurement Cluster
 *
 * COMPONENT:          RelativeHumidityMeasurement.h
 *
 * DESCRIPTION:        Header for Relative Humidity Measurement Cluster
 *
 *****************************************************************************/

#ifndef RELATIVE_HUMIDITY_MEASUREMENT_H
#define RELATIVE_HUMIDITY_MEASUREMENT_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef CLD_RHMEAS_CLUSTER_REVISION
    #define CLD_RHMEAS_CLUSTER_REVISION        1
#endif

#ifndef CLD_RHMEAS_FEATURE_MAP
    #define CLD_RHMEAS_FEATURE_MAP             0
#endif
/****************************************************************************/
/*       Relative Humidity Measurement Cluster - Optional Attributes        */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the relative humidity measurement cluster.                 */
/****************************************************************************/

/* Enable the optional Tolerance attribute */
//#define CLD_RHMEAS_ATTR_TOLERANCE

/* Enable the optional Attribute Reporting Status attribute */
//#define CLD_RHMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS

/* End of optional attributes */


/* Cluster ID's */
#define MEASUREMENT_AND_SENSING_CLUSTER_ID_RELATIVE_HUMIDITY_MEASUREMENT  0x0405


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Relative Humidity Measurement Information attribute set attribute ID's (4.7.2.2.1) */
    E_CLD_RHMEAS_ATTR_ID_MEASURED_VALUE          = 0x0000,  /* Mandatory */
    E_CLD_RHMEAS_ATTR_ID_MIN_MEASURED_VALUE,                /* Mandatory */
    E_CLD_RHMEAS_ATTR_ID_MAX_MEASURED_VALUE,                /* Mandatory */
    E_CLD_RHMEAS_ATTR_ID_TOLERANCE,
} teCLD_RHM_ClusterID;


/* Relative Humidity Measurement Cluster */
typedef struct
{
#ifdef RELATIVE_HUMIDITY_MEASUREMENT_SERVER    
    zuint16                u16MeasuredValue;

    zuint16                u16MinMeasuredValue;

    zuint16                u16MaxMeasuredValue;

#ifdef CLD_RHMEAS_ATTR_TOLERANCE
    zuint16                u16Tolerance;
#endif

#ifdef CLD_RHMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS
    zenum8                 u8AttributeReportingStatus;
#endif
#endif
    zbmap32                u32FeatureMap;
    
    zuint16                u16ClusterRevision;
} tsCLD_RelativeHumidityMeasurement;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_RelativeHumidityMeasurementCreateRelativeHumidityMeasurement(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZCL_ClusterDefinition sCLD_RelativeHumidityMeasurement;
extern uint8 au8RelativeHumidityMeasurementAttributeControlBits[];
extern const tsZCL_AttributeDefinition asCLD_RelativeHumidityMeasurementClusterAttributeDefinitions[];
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* RELATIVE_HUMIDITY_MEASUREMENT_H */
