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


/*
* Copyright 2016-2017 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef CONCENTRATION_MEASUREMENT_H
#define CONCENTRATION_MEASUREMENT_H

/*!
\file       ConcentrationMeasurement.h
\brief      Header for Concentration Measurement Cluster
*/

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/     
#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef CLD_CONCMEAS_CLUSTER_REVISION
    #define CLD_CONCMEAS_CLUSTER_REVISION        2
#endif

#ifndef CLD_CONCMEAS_FEATURE_MAP
    #define CLD_CONCMEAS_FEATURE_MAP             0
#endif
/****************************************************************************/
/*       Concentration Measurement Cluster - Optional Attributes              */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the concentration measurement cluster.                       */
/****************************************************************************/

/* Enable the optional Tolerance attribute */
//#define CLD_CONCMEAS_ATTR_TOLERANCE

/* Enable the optional Attribute Reporting Status attribute */
//#define CLD_CONCMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS

/* End of optional attributes */


/* Cluster ID's */
#define MEASUREMENT_AND_SENSING_CLUSTER_ID_CONCENTRATION_CARBON_DIOXIDE_MEASUREMENT  0x040D


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Concentration Measurement Information attribute set attribute ID's (4.13.2.1) */
    E_CLD_CONCMEAS_ATTR_ID_MEASURED_VALUE          = 0x0000,  /* Mandatory */
    E_CLD_CONCMEAS_ATTR_ID_MIN_MEASURED_VALUE,                /* Mandatory */
    E_CLD_CONCMEAS_ATTR_ID_MAX_MEASURED_VALUE,                /* Mandatory */
    E_CLD_CONCMEAS_ATTR_ID_TOLERANCE,
} teCLD_ConcentrationMeasurement_AttributeID;

/* Temperature Measurement Cluster */
typedef struct
{
#ifdef CONCENTRATION_MEASUREMENT_SERVER    
    zint16                 i16MeasuredValue;

    zint16                 i16MinMeasuredValue;

    zint16                 i16MaxMeasuredValue;

#ifdef CLD_CONCMEAS_ATTR_TOLERANCE
    zuint16                u16Tolerance;
#endif

#ifdef CLD_CONCMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS
    zenum8                 u8AttributeReportingStatus;
#endif
#endif
    zbmap32                u32FeatureMap;

    zuint16                u16ClusterRevision;
} tsCLD_ConcentrationMeasurement;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ConcentrationMeasurementCreateConcentrationMeasurement(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern const tsZCL_AttributeDefinition asCLD_CarbonDioxideConcentrationMeasurementClusterAttributeDefinitions[];
extern tsZCL_ClusterDefinition sCLD_CarbonDioxideMeasurement;
extern uint8 au8CarbonDioxideConcentrationMeasurementAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* CONCENTRATION_MEASUREMENT_H */
