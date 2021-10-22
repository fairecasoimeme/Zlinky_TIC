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

/*!=============================================================================
\file       ConcentrationMeasurement.c
\brief      Concentration Measurement cluster definition
==============================================================================*/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "ConcentrationMeasurement.h"
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
const tsZCL_AttributeDefinition asCLD_CarbonDioxideConcentrationMeasurementClusterAttributeDefinitions[] = {
#ifdef CONCENTRATION_MEASUREMENT_SERVER
        {E_CLD_CONCMEAS_ATTR_ID_MEASURED_VALUE,                 (E_ZCL_AF_RD|E_ZCL_AF_RP),      E_ZCL_INT16,    (uint32)(&((tsCLD_ConcentrationMeasurement*)(0))->i16MeasuredValue),      0},   /* Mandatory */

        {E_CLD_CONCMEAS_ATTR_ID_MIN_MEASURED_VALUE,             E_ZCL_AF_RD,                    E_ZCL_INT16,    (uint32)(&((tsCLD_ConcentrationMeasurement*)(0))->i16MinMeasuredValue),   0},/* Mandatory */

        {E_CLD_CONCMEAS_ATTR_ID_MAX_MEASURED_VALUE,             E_ZCL_AF_RD,                    E_ZCL_INT16,    (uint32)(&((tsCLD_ConcentrationMeasurement*)(0))->i16MaxMeasuredValue),   0},/* Mandatory */

        #ifdef CLD_CONCMEAS_ATTR_TOLERANCE
            {E_CLD_CONCMEAS_ATTR_ID_TOLERANCE,                  (E_ZCL_AF_RD|E_ZCL_AF_RP),       E_ZCL_UINT16,   (uint32)(&((tsCLD_ConcentrationMeasurement*)(0))->u16Tolerance),          0},
        #endif
  
#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                     (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_ConcentrationMeasurement*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
            
        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_UINT16,   (uint32)(&((tsCLD_ConcentrationMeasurement*)(0))->u16ClusterRevision),  0},   /* Mandatory  */
        
    #if (defined CONCENTRATION_MEASUREMENT_SERVER) && (defined CLD_CONCMEAS_ATTR_ATTRIBUTE_REPORTING_STATUS)
        {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,      (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_ENUM8,    (uint32)(&((tsCLD_ConcentrationMeasurement*)(0))->u8AttributeReportingStatus), 0},  /* Optional */
    #endif      
};

tsZCL_ClusterDefinition sCLD_CarbonDioxideMeasurement = {
        MEASUREMENT_AND_SENSING_CLUSTER_ID_CONCENTRATION_CARBON_DIOXIDE_MEASUREMENT,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_CarbonDioxideConcentrationMeasurementClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_CarbonDioxideConcentrationMeasurementClusterAttributeDefinitions,
        NULL
};

uint8 au8CarbonDioxideConcentrationMeasurementAttributeControlBits[(sizeof(asCLD_CarbonDioxideConcentrationMeasurementClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_ConcentrationMeasurementCreateConcentrationMeasurement
 **
 ** DESCRIPTION:
 ** Creates a concentration measurement cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ConcentrationMeasurementCreateConcentrationMeasurement(
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

    /* Create an instance of a concentration measurement cluster */
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
        #ifdef CONCENTRATION_MEASUREMENT_SERVER    
            /* Initializing these values to invalid as ZCL spec does not mention the defualt */
            ((tsCLD_ConcentrationMeasurement*)pvEndPointSharedStructPtr)->i16MinMeasuredValue = 0x8000;
            ((tsCLD_ConcentrationMeasurement*)pvEndPointSharedStructPtr)->i16MaxMeasuredValue = 0x8000;
        #endif
            ((tsCLD_ConcentrationMeasurement*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_CONCMEAS_FEATURE_MAP;

            ((tsCLD_ConcentrationMeasurement*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_CONCMEAS_CLUSTER_REVISION;
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

