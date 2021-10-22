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
 * MODULE:             Pressure Measurement Cluster
 *
 * COMPONENT:          PressureMeasurement.h
 *
 * DESCRIPTION:        Header for Pressure Measurement Cluster
 *
 *****************************************************************************/

#ifndef PRESSURE_MEASUREMENT_H
#define PRESSURE_MEASUREMENT_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef CLD_PRESSUREMEAS_CLUSTER_REVISION
    #define CLD_PRESSUREMEAS_CLUSTER_REVISION        2
#endif

#ifndef CLD_PRESSUREMEAS_FEATURE_MAP
    #define CLD_PRESSUREMEAS_FEATURE_MAP             0
#endif
/****************************************************************************/
/*       Pressure Measurement Cluster - Optional Attributes              */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the Pressure measurement cluster.                       */
/****************************************************************************/

/* Enable the optional Tolerance attribute */
//#define CLD_PRESSUREMEAS_ATTR_TOLERANCE

/* Enable the optional Attribute Reporting Status attribute */
//#define CLD_PRESSUREMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS

/* End of optional attributes */


/* Cluster ID's */
#define MEASUREMENT_AND_SENSING_CLUSTER_ID_PRESSURE_MEASUREMENT  0x0403


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Pressure Measurement Information attribute set attribute ID's (4.5.2.2.1) */
    E_CLD_PRESSUREMEAS_ATTR_ID_MEASURED_VALUE          = 0x0000,  /* Mandatory */
    E_CLD_PRESSUREMEAS_ATTR_ID_MIN_MEASURED_VALUE,                /* Mandatory */
    E_CLD_PRESSUREMEAS_ATTR_ID_MAX_MEASURED_VALUE,                /* Mandatory */
    E_CLD_PRESSUREMEAS_ATTR_ID_TOLERANCE,
} teCLD_PM_ClusterID;

/* Pressure Measurement Cluster */
typedef struct
{
#ifdef PRESSURE_MEASUREMENT_SERVER

    zint16                 i16MeasuredValue;
        
    zint16                 i16MinMeasuredValue;
                           
    zint16                 i16MaxMeasuredValue;

#ifdef CLD_PRESSUREMEAS_ATTR_TOLERANCE
    zuint16                u16Tolerance;
#endif
    
#ifdef CLD_PRESSUREMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS
    zenum8                 u8AttributeReportingStatus;
#endif

#endif
    zbmap32                u32FeatureMap;

    zuint16                u16ClusterRevision;
} tsCLD_PressureMeasurement;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_PressureMeasurementCreatePressureMeasurement(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZCL_ClusterDefinition sCLD_PressureMeasurement;
extern uint8 au8PressureMeasurementAttributeControlBits[];
extern const tsZCL_AttributeDefinition asCLD_PressureMeasurementClusterAttributeDefinitions[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* PRESSURE_MEASUREMENT_H */
